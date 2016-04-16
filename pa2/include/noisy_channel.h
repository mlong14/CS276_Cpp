#ifndef PA2_NOISY_CHANNEL_H_
#define PA2_NOISY_CHANNEL_H_

#include "edit_cost_model.h"
#include <string>

class NoisyChannel {
	public:
		static NoisyChannel* getInstance(std::string input = "");
		static void destroy();
		void setProbType(std::string type = "uniform");

		friend std::ostream& operator<< (std::ostream &os, const NoisyChannel &nc);
		friend std::istream& operator>> (std::istream &is, NoisyChannel &nc);
	private:
		NoisyChannel();
		NoisyChannel(std::string input);
		~NoisyChannel();

		static NoisyChannel *_nc;
	public:
		EditCostModel *_ec;
		UniformCostModel *_um;
		EmpiricalCostModel *_em;
};

#endif //PA2_NOISY_CHANNEL_H_
