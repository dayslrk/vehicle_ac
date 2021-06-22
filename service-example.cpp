#include <iomanip>
#include <iostream>
#include <sstream>

#include <vsomeip/vsomeip.hpp>

#include "ABAC.h"
#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0421
#define SAMPLE_EVENTGROUP_ID 0x001
#define SAMPLE_EVENT_ID 0x002

bool CAN_Transmit(uint8_t *pData){
	ABAC_Command Command = (ABAC_Command)pData[0];
	//没有lockresource
	switch (Command) {
		case ACCESS:
			break;
		case INFORMATION:
			break;
		case DECISIONRESP:
			break;
		case UNLOCK_RESOURCE:
			break;
		case NONCE:
			break;
	}
	return true;
}


std::shared_ptr< vsomeip::application > app;

void on_message(const std::shared_ptr<vsomeip::message> &_request) {
    Policies PolicyObj(CAN_Transmit);
    std::shared_ptr<vsomeip::payload> its_payload = _request->get_payload();
    vsomeip::length_t l = its_payload->get_length();
    Decision ret;
    std::cout<<"payload length is "<<l<<std::endl;
    //处理访问请求
    if(l==20)
    {
        int i=0;
        uint8_t subid_high=(uint8_t)*(its_payload->get_data()+i);
        uint8_t subid_low=(uint8_t)*(its_payload->get_data()+i+1);
	std::cout<<"high 8: "<<subid_high<<"low 8:"<<subid_low<<std::endl;
        uint8_t subval_high=(uint8_t)*(its_payload->get_data()+i+2);
        uint8_t subval_low=(uint8_t)*(its_payload->get_data()+i+3);
        //AttributeCategory subcat=(AttributeCategory)*(its_payload->get_data()+i+4);
        const Attribute role_mechanic((subid_high<<8) | subid_low, (subval_high<<8)|subval_low, ACCESS_SUBJECT);

        i=5;
        uint8_t actid_high=(uint8_t)*(its_payload->get_data()+i);
        uint8_t actid_low=(uint8_t)*(its_payload->get_data()+i+1);
        uint8_t actval_high=(uint8_t)*(its_payload->get_data()+i+2);
        uint8_t actval_low=(uint8_t)*(its_payload->get_data()+i+3);
        //AttributeCategory actcat=(AttributeCategory)*(its_payload->get_data()+i+4);
	const Attribute action_read((actid_high<<8) | actid_low, (actval_high<<8)|actval_low, ACTION);
        i=10;
        uint8_t resid_high=(uint8_t)*(its_payload->get_data()+i);
        uint8_t resid_low=(uint8_t)*(its_payload->get_data()+i+1);
        uint8_t resval_high=(uint8_t)*(its_payload->get_data()+i+2);
        uint8_t resval_low=(uint8_t)*(its_payload->get_data()+i+3);
        //AttributeCategory rescat=(AttributeCategory)*(its_payload->get_data()+i+4);
	const Attribute resourceType_diagnosisData((resid_high<<8) | resid_low, (resval_high<<8)|resval_low, RESOURCE);
        i=15;
        uint8_t envid_high=(uint8_t)*(its_payload->get_data()+i);
        uint8_t envid_low=(uint8_t)*(its_payload->get_data()+i+1);
        uint8_t envval_high=(uint8_t)*(its_payload->get_data()+i+2);
        uint8_t envval_low=(uint8_t)*(its_payload->get_data()+i+3);
        //AttributeCategory envcat=(AttributeCategory)*(its_payload->get_data()+i+4);
	const Attribute testerConnectionStatus_connected((envid_high<<8) | envid_low, (envval_high<<8)|envval_low, ENVIRONMENT);

    Policies PolicyObj(CAN_Transmit);
	PolicyObj.sendRequest(role_mechanic,action_read,resourceType_diagnosisData);
	PolicyObj.setInformation(testerConnectionStatus_connected);
	ret = PolicyObj.request();

        
    }

    // Get payload
    std::stringstream ss;
    //原来是0-l，这里改成10-11，为资源的id
    for (vsomeip::length_t i=10; i<12; i++) {
        
       ss << std::setw(2) << std::setfill('0') << std::hex
          << (unsigned int)*(its_payload->get_data()+i);
    }
    //最后的访问决定
    std::cout << "SERVICE: Received message with Client/Session ["
        << std::setw(4) << std::setfill('0') << std::hex << _request->get_client() << "/"
        << std::setw(4) << std::setfill('0') << std::hex << _request->get_session() << "] "
        <<"request for "<< ss.str() <<", "<<"PDP decision is "<<ret<<std::endl;
    



    // Send response
    std::shared_ptr<vsomeip::message> its_response = vsomeip::runtime::get()->create_response(_request);
    its_payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> its_payload_data;
    for (int i=9; i>=0; i--) {
        its_payload_data.push_back(i % 256);
    }
    its_payload->set_data(its_payload_data);
    its_response->set_payload(its_payload);
    app->send(its_response);

    // Notify event
    const vsomeip::byte_t its_data[] = { 0x10 };
	its_payload = vsomeip::runtime::get()->create_payload();
	its_payload->set_data(its_data, sizeof(its_data));
    app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_payload);
}

int main() {

    app = vsomeip::runtime::get()->create_application("World");
    
	std::set<vsomeip::eventgroup_t> its_groups;
	its_groups.insert(SAMPLE_EVENTGROUP_ID);

    app->init();
    app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);
    app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups);
    app->start();
}
