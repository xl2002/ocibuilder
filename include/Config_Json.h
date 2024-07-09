#ifndef CONFIG_JSON_H
#define CONFIG_JSON_H

class ContainersConfig{

};

class EngineConfig{

};

class MachineConfig{

};

class NetworkConfig{

};

class SecretConfig{

};

class ConfigMapConfig{

};


class FarmConfig{

};

class Configdetails{
    public:
    ContainersConfig*    Containers=nullptr;
    EngineConfig*        Engine=nullptr;
    MachineConfig*       Machine=nullptr;
    NetworkConfig*       Network=nullptr;
    SecretConfig*        Secrets=nullptr;
    ConfigMapConfig*     ConfigMaps=nullptr;
    FarmConfig*          Farms=nullptr;


    void CheckCgroupsAndAdjustConfig();
};

Configdetails ConfigdetailsDefault();

#endif