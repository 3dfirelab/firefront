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

class ANNPropagationModel: public PropagationModel {
    /*! name the model */
    static const std::string name;
    /*! boolean for initialization */
    static int isInitialized;
    /*! properties needed by the model */
   /* size_t slope;
    size_t normalWind;
    size_t Rhod;
    size_t sd;
	size_t Ta;*/
    /*! coefficients needed by the model */
    double windReductionFactor;
    Network annNetwork; // Neural network instance for the model

public:
    ANNPropagationModel(const int& = 0, DataBroker* db = nullptr);
    virtual ~ANNPropagationModel();
    std::string getName();
    double getSpeed(double*);

    void loadNetwork(const std::string& filename); // Method to load network configuration
};

PropagationModel* getANNPropagationModelModel(const int& = 0, DataBroker* db = nullptr);

} /* namespace libforefire */

using namespace std;
namespace libforefire {

/* name of the model */
const string ANNPropagationModel::name = "ANNPropagationModel";

/* instantiation */
PropagationModel* getANNPropagationModelModel(const int & mindex, DataBroker* db) {
    return new ANNPropagationModel(mindex, db);
}

/* registration */
int ANNPropagationModel::isInitialized =
    FireDomain::registerPropagationModelInstantiator(name, getANNPropagationModelModel);

/* constructor */
ANNPropagationModel::ANNPropagationModel(const int & mindex, DataBroker* db)
: PropagationModel(mindex, db) {
    std::string annPath = params->getParameter("FFANNPropagationModelPath");
    annNetwork.loadFromFile(annPath.c_str());
    properties = new double[annNetwork.inputNames.size()];
    for (size_t i = 0; i < annNetwork.inputNames.size(); ++i) {
        registerProperty(annNetwork.inputNames[i]);
    }
    dataBroker->registerPropagationModel(this);
}



ANNPropagationModel::~ANNPropagationModel() {
    if (properties) {
        delete[] properties;
    }
}

/* accessor to the name of the model */
string ANNPropagationModel::getName(){
    return name;
}


double ANNPropagationModel::getSpeed(double* valueOf) {
    std::vector<float> inputs(numProperties);
    std::vector<float> outputs = annNetwork.processInput(inputs);
    return static_cast<double>(abs(outputs[0]));
}


}