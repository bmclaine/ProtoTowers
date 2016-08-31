#pragma once 

#include <string>

struct Property{

public:
	Property(){
		name = "";
		value = "";
	}

	Property(std::string _name, std::string _value){
		name = _name;
		value = _value;
	}

	std::string name;
	std::string value;

};