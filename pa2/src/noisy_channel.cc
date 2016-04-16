
#include "noisy_channel.h"
#include "edit_cost_model.h"
#include "utils.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>

NoisyChannel *NoisyChannel::_nc = nullptr;

NoisyChannel* NoisyChannel::getInstance(std::string input) {
	if (!_nc) {
		if (input != "") {
			_nc = new NoisyChannel(input);
			_nc->setProbType("uniform");
		}
		else {
			_nc = new NoisyChannel();
		}
	}
	return _nc;
}

void NoisyChannel::destroy() {
	delete _nc;
	_nc = nullptr;
}

NoisyChannel::NoisyChannel() {
	_um = new UniformCostModel();
	_em = new EmpiricalCostModel();
};

NoisyChannel::NoisyChannel(std::string input) {
	_um = new UniformCostModel();
	_em = new EmpiricalCostModel(input);
}

NoisyChannel::~NoisyChannel() {
	delete _um;
	delete _em;
}

void NoisyChannel::setProbType(std::string type) {
	if (type == "uniform") {
		_ec = _um;
	}
	else if (type == "empirical") {
		_ec = _em;
	}
	else {
		std::cerr << "Unknown Cost Model type " << type << std::endl;
		exit(1);
	}
}

std::ostream& operator<< (std::ostream &os, const NoisyChannel &nc) {
	os << *nc._um;
	os << *nc._em;

	return os;
}

std::istream& operator>> (std::istream &is, NoisyChannel &nc) {
	is >> *nc._um;
	is >> *nc._em;
	
	return is;
}
