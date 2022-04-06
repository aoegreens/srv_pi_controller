#include <memory>
#include <cstdlib>
#include <restbed>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include "base64.h"
#include <wiringPi.h>
#include <string>
#include <sstream>

using namespace std;
using namespace restbed;
using json = nlohmann::json;

struct Auth
{
    string type;
    string username;
    string password;
};

struct RequiredParameter
{
    string upstreamKey;
    string name;
    vector<string> values;
    string defaultVal;
};

Auth GetAuth(const shared_ptr< Session > session)
{
    Auth ret;
    string authorization = session->get_request()->get_header("Authorization");
    size_t breakPos = authorization.find(' ');
    ret.type = authorization.substr(0, breakPos);
    if (ret.type != "Basic")
        return ret;
    string b64 = authorization.substr(++breakPos);
    string b64Decoded;
    macaron::Base64::Decode(b64, b64Decoded);
    breakPos = b64Decoded.find(':');
    ret.username = b64Decoded.substr(0,breakPos);
    ret.password = b64Decoded.substr(++breakPos);
//    fprintf(stdout, "%s (%s:%s => %s)\n", authorization.c_str(), ret.type.c_str(), b64.c_str(), b64Decoded.c_str());
    fprintf(stdout, "Username: \"%s\"\nPassword: \"%s\"\n", ret.username.c_str(), ret.password.c_str());
    return ret;
}

typedef enum {
    STATE_UNKOWN = 0,
    ON,
    OFF,
    TOGGLE
} GPIOState;

typedef enum {
    DIRECTION_UNKOWN = 0,
    IN,
    OUT
} GPIODirection;

class GPIOManager
{
public:
    GPIOManager(uint8_t pin, GPIODirection direction=OUT) :
        m_pin(pin),
        m_initialized(false),
        m_state(STATE_UNKOWN),
        m_direction(direction)
    {
        Initialize();
    }
    
    virtual ~GPIOManager()
    {

    }

    virtual uint8_t GetPin() const
    {
        return m_pin;
    }

    virtual GPIODirection GetDirection() const
    {
        return m_direction;
    }

    virtual bool HasBeenInitialized() const
    {
        return m_initialized;
    }

    virtual void Initialize()
    {
        switch (m_direction)
        {
        case IN:
            pinMode(m_pin, INPUT);
            m_initialized = true;
            break;
        case OUT:
            pinMode(m_pin, OUTPUT);
            digitalWrite(m_pin, LOW);
            m_state = OFF;
            m_initialized = true;
            break;
        default:
            m_initialized = false;
            break;
        }
    }

    virtual float ReadInput()
    {
        if (m_direction != IN)
        {
            return 0.0f;
        }

        //TODO
        return 0.0f;
    }

    virtual GPIOState GetState() const
    {
        return m_state;
    }

    virtual GPIOState SetState(const GPIOState state)
    {
        if (m_direction != OUT)
        {
            return m_state;
        }

        switch (state)
        {
        case ON:
            digitalWrite(m_pin, HIGH);
            m_state = ON;
            break;
        case OFF:
            digitalWrite(m_pin, LOW);
            m_state = OFF;
            break;
        case TOGGLE:
            switch (m_state)
            {
            case ON:
                return SetState(OFF);
            case OFF:
                return SetState(ON);
            default:
                break;
            }
            break;
        default:
            return STATE_UNKOWN;
        }
        return m_state;
    }

protected:
    uint8_t m_pin;
    bool m_initialized;
    GPIOState m_state;
    GPIODirection m_direction;
};

class PinController
{
public:
    PinController()
    {
        
    }
    
    virtual ~PinController()
    {
        for (auto man : m_gpioManagers)
        {
            delete man;
        }
        m_gpioManagers.clear();
    }

    GPIOManager* GetGPIOManager(uint8_t pin, GPIODirection defaultDirection=OUT)
    {
        for (auto man : m_gpioManagers)
        {
            if (man->GetPin() == pin)
            {
                return man;
            }
        }
        GPIOManager* newManager = new GPIOManager(pin, defaultDirection);
        m_gpioManagers.push_back(newManager);
        return newManager;
    }


    virtual float ReadGPIOInput(uint8_t pin)
    {
        return GetGPIOManager(pin, IN)->ReadInput();
    }

    virtual GPIOState GetGPIOState(uint8_t pin)
    {
        return GetGPIOManager(pin, OUT)->GetState();
    }

    virtual GPIOState SetGPIOState(uint8_t pin, const GPIOState state)
    {
        return GetGPIOManager(pin, OUT)->SetState(state);
    }

    virtual GPIODirection GetGPIODirection(uint8_t pin)
    {
        return GetGPIOManager(pin, OUT)->GetDirection();
    }

protected:
    std::vector< GPIOManager* > m_gpioManagers;
};

PinController g_pinController;


void pi_gpio_get(const shared_ptr< Session > session)
{
    auto request = session->get_request();

#if 1
    string parameters;
    for (const auto& param : request->get_query_parameters())
    {
        parameters += "{"+param.first+" : "+param.second+"} ";
    }
    fprintf(stdout, "Processing gpio request for: {\n    %s\n}\n", parameters.c_str());
#endif

    if (!request->has_query_parameter("pin"))
    {
        session->close(400, "You must specify a pin.");
        session->erase();
        return;
    }

    stringstream ret;
    uint8_t pin = stoi(request->get_query_parameter("pin"));
    switch (g_pinController.GetGPIODirection(pin))
    {
    case IN:
        ret << g_pinController.ReadGPIOInput(pin);
        break;
    case OUT:
        switch (g_pinController.GetGPIOState(pin))
        {
        case ON:
            ret << "ON";
            break;
        case OFF:
            ret << "OFF";
            break;
        default:
            ret << "ERROR ON PIN " << pin;
            break;
        }
        break;
    default:
        ret << "ERROR ON PIN " << pin;
        break;
    }

    session->close(OK, ret.str());
    session->erase();
}

void pi_gpio_set(const shared_ptr< Session > session)
{
    const auto request = session->get_request();
    int contentLength = request->get_header("Content-Length", 0);

    session->fetch(contentLength, [ request, contentLength ](const shared_ptr< Session > l_session, const Bytes & l_body)
    {
#if 1
        string parameters;
        for (const auto& param : request->get_query_parameters())
        {
            parameters += "{"+param.first+" : "+param.second+"} ";
        }
        fprintf(stdout, "Processing gpio request for: {\n    %s\n}\nand l_body (%d): {\n    %.*s\n}\n", parameters.c_str(), contentLength, (int) l_body.size(), l_body.data());
#endif

        char* requestJson = new char[l_body.size()];
        sprintf(requestJson, "%.*s", (int) l_body.size(), l_body.data());
        string jStr(requestJson);
        json requestData = json::parse(jStr, nullptr, false);
        if (requestData.is_discarded())
        {
            fprintf(stdout, "Could not parse:\n%.*s\n", (int) l_body.size(), l_body.data());
            l_session->close(400, "Could not parse request. Please submit request as valid json.");
            l_session->erase();
            return;
        }
        delete[] requestJson;
        const vector<RequiredParameter> requiredParameters = {
                {"pin", "pin", {}, ""}, //the wiringPi pin to use.
                {"state", "state", {}, ""}, //the state to set; "ON", "OFF". '"TOGGLE'.
        };

        json upstreamRequestBody;
        for (const auto& req : requiredParameters)
        {
            if (!requestData.contains(req.name))
            {
                if(req.defaultVal.empty())
                {
                    l_session->close(400, "Please specify \"" + req.name + "\"");
                    l_session->erase();
                    return;
                }
            }
        }

        GPIOState toSet = TOGGLE;
        if (requestData["state"] == "ON")
        {
            toSet = ON;
        }
        else if (requestData["state"] == "OFF")
        {
            toSet = OFF;
        }

        uint8_t pin = stoi(requestData["pin"]);

        g_pinController.SetGPIOState(pin, toSet);
        
        l_session->close(OK, "complete.");
        l_session->erase();
    });
}

int main(const int, const char**)
{
    auto gpio_get = make_shared< Resource >();
    gpio_get->set_path("/v1/gpio/get");
    gpio_get->set_method_handler("GET", pi_gpio_get);

    auto gpio_set = make_shared< Resource >();
    gpio_set->set_path("/v1/gpio/set");
    gpio_set->set_method_handler("POST", pi_gpio_set);

    auto settings = make_shared< Settings >();
    settings->set_port(80);
    settings->set_default_header("Connection", "close");

    Service service;
    service.publish(gpio_get);
    service.publish(gpio_set);
    service.start(settings);

    return EXIT_SUCCESS;
}