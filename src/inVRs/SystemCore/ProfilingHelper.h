/*
 * ProfilingHelper.h
 *
 *  Created on: May 19, 2009
 *      Author: rlander
 */

#ifndef PROFILINGHELPER_H_
#define PROFILINGHELPER_H_

#include <string>
#include <vector>

namespace inVRsUtilities {

class ProfilingHelper {
public:
	ProfilingHelper();
	~ProfilingHelper();

	void setLogFile(std::string file);

	void start();
	void step(std::string name);
	void interrupt();
	void end();

private:
	ProfilingHelper(const ProfilingHelper& src);
	ProfilingHelper& operator=(const ProfilingHelper& rhs);

	FILE* logFile;

	bool firstRun;
	bool invalidStep;
	double lastTime;
	int index;
	int numValues;
	std::vector<double> values;
	std::vector<std::string> names;

	void initialize(std::string logFile);
}; // ProfilingHelper


} // inVRsUtilities

#endif /* PROFILINGHELPER_H_ */
