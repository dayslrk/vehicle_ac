#include <iomanip>
#include <iostream>
#include <sstream>

#include <condition_variable>
#include <thread>
#include <vsomeip/vsomeip.hpp>

#include "ABAC.h"
#include "Attribute.h"
#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0421
#define SAMPLE_EVENTGROUP_ID 0x001
#define SAMPLE_EVENT_ID 0x002

std::shared_ptr< vsomeip::application > app;
std::mutex mutex;
std::condition_variable condition;



//待发送的消息
const Attribute role_mechanic(0x29A7, 0xBCD8, ACCESS_SUBJECT);
const Attribute action_read(0x418C, 0xECAE, ACTION);
const Attribute resourceType_diagnosisData(0x6F47, 0x2092, RESOURCE);
const Attribute testerConnectionStatus_connected(0xE6D1, 0x06AA, ENVIRONMENT);
std::vector<uint8_t> generate_request_payload(Attribute role_mechanic,Attribute action_read,Attribute resourceType_diagnosisData,Attribute testerConnectionStatus_connected)
{
    std::vector<uint8_t> its_payload_data;
    its_payload_data.push_back((role_mechanic.getID()>>8)&0xFF);
    its_payload_data.push_back(role_mechanic.getID()&0xFF);
    its_payload_data.push_back((role_mechanic.getValue()>>8)&0xFF);
    its_payload_data.push_back(role_mechanic.getValue()&0xFF);
    its_payload_data.push_back(role_mechanic.getCategory());

    its_payload_data.push_back((action_read.getID()>>8)&0xFF);
    its_payload_data.push_back(action_read.getID()&0xFF);
    its_payload_data.push_back((action_read.getValue()>>8)&0xFF);
    its_payload_data.push_back(action_read.getValue()&0xFF);
    its_payload_data.push_back(action_read.getCategory());

    its_payload_data.push_back((resourceType_diagnosisData.getID()>>8)&0xFF);
    its_payload_data.push_back(resourceType_diagnosisData.getID()&0xFF);
    its_payload_data.push_back((resourceType_diagnosisData.getValue()>>8)&0xFF);
    its_payload_data.push_back(resourceType_diagnosisData.getValue()&0xFF);
    its_payload_data.push_back(resourceType_diagnosisData.getCategory());
    
    its_payload_data.push_back((testerConnectionStatus_connected.getID()>>8)&0xFF);
    its_payload_data.push_back(testerConnectionStatus_connected.getID()&0xFF);
    its_payload_data.push_back((testerConnectionStatus_connected.getValue()>>8)&0xFF);
    its_payload_data.push_back(testerConnectionStatus_connected.getValue()&0xFF);
    its_payload_data.push_back(testerConnectionStatus_connected.getCategory());
    return its_payload_data;

}	
	
void run() {
    std::unique_lock<std::mutex> its_lock(mutex);
    condition.wait(its_lock);

    std::set<vsomeip::eventgroup_t> its_groups;
    its_groups.insert(SAMPLE_EVENTGROUP_ID);
    app->request_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups);
    app->subscribe(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);

    // Send Request
    std::shared_ptr< vsomeip::message > request;
    request = vsomeip::runtime::get()->create_request();
    request->set_service(SAMPLE_SERVICE_ID);
    request->set_instance(SAMPLE_INSTANCE_ID);
    request->set_method(SAMPLE_METHOD_ID);

    std::shared_ptr< vsomeip::payload > its_payload = vsomeip::runtime::get()->create_payload();
    std::vector<uint8_t> its_payload_data;
    its_payload_data=generate_request_payload(role_mechanic,action_read,resourceType_diagnosisData,testerConnectionStatus_connected);
    // its_payload_data.push_back((role_mechanic.getID()>>8)&0xFF);
    // its_payload_data.push_back(role_mechanic.getID()&0xFF);
    // its_payload_data.push_back((role_mechanic.getValue()>>8)&0xFF);
    // its_payload_data.push_back(role_mechanic.getValue()&0xFF);
    // its_payload_data.push_back(role_mechanic.getCategory());

    // its_payload_data.push_back((action_read.getID()>>8)&0xFF);
    // its_payload_data.push_back(action_read.getID()&0xFF);
    // its_payload_data.push_back((action_read.getValue()>>8)&0xFF);
    // its_payload_data.push_back(action_read.getValue()&0xFF);
    // its_payload_data.push_back(action_read.getCategory());

    // its_payload_data.push_back((resourceType_diagnosisData.getID()>>8)&0xFF);
    // its_payload_data.push_back(resourceType_diagnosisData.getID()&0xFF);
    // its_payload_data.push_back((resourceType_diagnosisData.getValue()>>8)&0xFF);
    // its_payload_data.push_back(resourceType_diagnosisData.getValue()&0xFF);
    // its_payload_data.push_back(resourceType_diagnosisData.getCategory());
    
    // its_payload_data.push_back((testerConnectionStatus_connected.getID()>>8)&0xFF);
    // its_payload_data.push_back(testerConnectionStatus_connected.getID()&0xFF);
    // its_payload_data.push_back((testerConnectionStatus_connected.getValue()>>8)&0xFF);
    // its_payload_data.push_back(testerConnectionStatus_connected.getValue()&0xFF);
    // its_payload_data.push_back(testerConnectionStatus_connected.getCategory());

    its_payload->set_data(its_payload_data);
    request->set_payload(its_payload);
    app->send(request);
}

void on_message(const std::shared_ptr<vsomeip::message> &_response) {
    
    std::stringstream its_message;
    its_message << "CLIENT: received a notification for event ["
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_service() << "."
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_instance() << "."
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_method() << "] to Client/Session ["
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_client() << "/"
            << std::setw(4) << std::setfill('0') << std::hex
            << _response->get_session()
            << "] = ";
    std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
    its_message << "(" << std::dec << its_payload->get_length() << ") ";
    for (uint32_t i = 0; i < its_payload->get_length(); ++i)
        its_message << std::hex << std::setw(2) << std::setfill('0')
            << (int) its_payload->get_data()[i] << " ";
    std::cout << its_message.str() << std::endl;

    // std::shared_ptr<vsomeip::payload> its_payload = _response->get_payload();
    // vsomeip::length_t l = its_payload->get_length();

    // // Get payload
    // std::stringstream ss;
    // for (vsomeip::length_t i=0; i<l; i++) {
    //     ss << std::setw(2) << std::setfill('0') << std::hex
    //         << (int)*(its_payload->get_data()+i) << " ";
    // }

    // std::cout << "CLIENT: Received message with Client/Session ["
    //           << std::setw(4) << std::setfill('0') << std::hex << _response->get_client() << "/"
    //           << std::setw(4) << std::setfill('0') << std::hex << _response->get_session() << "] "
    //           << ss.str() << std::endl;
}
    

void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
    std::cout << "CLIENT: Service ["
              << std::setw(4) << std::setfill('0') << std::hex << _service << "." << _instance
              << "] is "
              << (_is_available ? "available." : "NOT available.")
              << std::endl;
    if (_is_available) { condition.notify_one(); }
}

int main() {

    app = vsomeip::runtime::get()->create_application("Hello");
    
    app->init();
    app->register_availability_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, on_availability);
    app->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    // app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);
    app->register_message_handler(vsomeip::ANY_SERVICE, vsomeip::ANY_INSTANCE, vsomeip::ANY_METHOD, on_message);
    std::thread sender(run);
    app->start();
}
