#include <iostream>
#include <Phidget21/phidget21.h>

const char* GetPhidgetErrorString(int errorCode)
{
    const char* errorString;
    CPhidget_getErrorDescription(errorCode, &errorString);

    return errorString;
}

#define TRY_PHIDGET(op) { int result{0}; if ((result = (op)) != EPHIDGET_OK) { \
    throw std::runtime_error(GetPhidgetErrorString(result)); } }

int AttachHandler(CPhidgetHandle phidget, void* /*ignored*/)
{
    int serialNo;
    TRY_PHIDGET(CPhidget_getSerialNumber(phidget, &serialNo));

    CPhidgetTemperatureSensorHandle sensor{0};
    CPhidgetTemperatureSensor_create(&sensor);
    TRY_PHIDGET(CPhidget_open(reinterpret_cast<CPhidgetHandle>(sensor), serialNo));
    TRY_PHIDGET(CPhidget_waitForAttachment(reinterpret_cast<CPhidgetHandle>(sensor), 5000));

    return 0;
}


int main(int argc, char* argv[]) {
    CPhidgetManagerHandle handle{0};

    TRY_PHIDGET(CPhidgetManager_create(&handle));
    TRY_PHIDGET(CPhidgetManager_set_OnAttach_Handler(handle, AttachHandler, nullptr));
    TRY_PHIDGET(CPhidgetManager_open(handle));

    return 0;
}
