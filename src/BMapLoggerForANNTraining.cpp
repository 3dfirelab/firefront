/*

Copyright (C) 2024 ForeFire Team, SPE, Universit� de Corse.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 US

 */

#include "PropagationModel.h"
#include "FireDomain.h"
#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>

#include "ANN.h"  // Include the ANN definitions

namespace libforefire {

class BMapLoggerForANNTraining: public PropagationModel {
    static const std::string name;
    static int isInitialized;

    Network annNetwork; // Neural network instance for the model
    std::ofstream csvfile;
public:
    BMapLoggerForANNTraining(const int& = 0, DataBroker* db = nullptr);
    virtual ~BMapLoggerForANNTraining();
    std::string getName();
    double getSpeed(double*);

    void loadNetwork(const std::string& filename); // Method to load network configuration
};

PropagationModel* getBMapLoggerForANNTrainingModel(const int& = 0, DataBroker* db = nullptr);

} /* namespace libforefire */

using namespace std;
namespace libforefire {

/* name of the model */
const string BMapLoggerForANNTraining::name = "BMapLoggerForANNTraining";
 
/* instantiation */
PropagationModel* getBMapLoggerForANNTrainingModel(const int & mindex, DataBroker* db) {
    return new BMapLoggerForANNTraining(mindex, db);
}

/* registration */
int BMapLoggerForANNTraining::isInitialized =
    FireDomain::registerPropagationModelInstantiator(name, getBMapLoggerForANNTrainingModel);

/* constructor */
BMapLoggerForANNTraining::BMapLoggerForANNTraining(const int & mindex, DataBroker* db)
: PropagationModel(mindex, db) {
    // Load network first to access names
    std::string annPath = params->getParameter("FFANNPropagationModelPath");
    std::string csvPath = params->getParameter("FFBMapLoggerCSVPath");
  
    
    annNetwork.loadFromFile(annPath.c_str());
    csvfile.open(csvPath); 
    properties = new double[annNetwork.inputNames.size() + 1];
    csvfile << "ROS";
    registerProperty("arrival_time_gradient");
    for (const auto& inputName : annNetwork.inputNames) {
        csvfile << ";"<< inputName ;
        
        registerProperty(inputName);
    }
    csvfile << std::endl;
    dataBroker->registerPropagationModel(this);
}


BMapLoggerForANNTraining::~BMapLoggerForANNTraining() {
    if (properties) {
        delete[] properties;
    }
        if (csvfile.is_open()) {
        csvfile.close();
    }
}

/* accessor to the name of the model */
string BMapLoggerForANNTraining::getName(){
    return name;
}


double BMapLoggerForANNTraining::getSpeed(double* valueOf) {
    double RosVal = 0.0;
    if (valueOf[0] > 0){
        RosVal = 1.0/valueOf[0]; 
    }
    csvfile << RosVal << ";";   
    for (size_t i = 0; i < annNetwork.inputNames.size()-1; ++i) {
        csvfile << valueOf[i] << ";";
    }
    csvfile << valueOf[annNetwork.inputNames.size()] << std::endl;

    return RosVal;
}


}