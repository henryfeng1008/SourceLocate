//
//  GateReadSort.h
//  GateReadSort
//
//  Created by Hanyu Feng on 8/7/17.
//  Copyright © 2017 Hanyu Feng. All rights reserved.
//

#ifndef GateReadSort_h
#define GateReadSort_h
using namespace std;
typedef struct{
    double x;
    double y;
    double z;
}pos;

typedef struct{
    int base;
    int rsector;
    int module;
    int submodule;
    int crystal;
    int layer;
}crystal;

#pragma pack(4)
typedef struct {
    int run_ID_1;
    int event_ID_1;
    int source_ID_1;
    pos  position_source_1;
    double time_stamp_source_1;
    double deposited_energy_1;
    pos position_1;
    crystal single_1;
    int Compton_phantom_1;
    int Compton_detectors_1;
    int Rayleigh_phantom_1;
    int Rayleigh_detectors_1;
    double scanner_axial_1;
    double scanner_angular_1;
    
    int run_ID_2;
    int event_ID_2;
    int source_ID_2;
    pos position_source_2;
    double time_stamp_source_2;
    double deposited_energy_2;
    pos position_2;
    crystal single_2;
    int Compton_phantom_2;
    int Compton_detectors_2;
    int Rayleigh_phantom_2;
    int Rayleigh_detectors_2;
    double scanner_axial_2;
    double scanner_angular_2;
   
}coincidence_struct;


typedef struct {
    long LOR;
    int crystal_1;
    int crystal_2;
    double time;
}Event;

typedef struct {
    long LOR;
    int crystal_1;
    int crystal_2;
}LUT;


void check_size(){
    cout<<"coincidence_struct, "<<sizeof(coincidence_struct)<<endl;
    cout<<"pos, "<<sizeof(pos)<<endl;
    cout<<"crystal, "<<sizeof(crystal)<<endl;
}


/*
void show_read(coincidence_struct *coincidence, int i){
    cout<<coincidence[i].run_ID_1<<endl;
    cout<<coincidence[i].event_ID_1<<endl;
    cout<<coincidence[i].source_ID_1<<endl;
    cout<<coincidence[i].position_source_1.x<<endl;
    cout<<coincidence[i].position_source_1.y<<endl;
    cout<<coincidence[i].position_source_1.z<<endl;
    cout<<coincidence[i].time_stamp_source_1<<endl;
    cout<<coincidence[i].deposited_energy_1<<endl;
    cout<<coincidence[i].position_1.x<<endl;
    cout<<coincidence[i].position_1.y<<endl;
    cout<<coincidence[i].position_1.z<<endl;
    cout<<coincidence[i].single_1.base<<endl;
    cout<<coincidence[i].single_1.rsector<<endl;
    cout<<coincidence[i].single_1.module<<endl;
    cout<<coincidence[i].single_1.submodule<<endl;
    cout<<coincidence[i].single_1.crystal<<endl;
    cout<<coincidence[i].single_1.layer<<endl;
    cout<<coincidence[i].Compton_phantom_1<<endl;
    cout<<coincidence[i].Compton_detectors_1<<endl;
    cout<<coincidence[i].Rayleigh_phantom_1<<endl;
    cout<<coincidence[i].Rayleigh_detectors_1<<endl;
    cout<<coincidence[i].scanner_axial_1<<endl;
    cout<<coincidence[i].scanner_angular_1<<endl;
 
    cout<<coincidence[i].run_ID_2<<endl;
    cout<<coincidence[i].event_ID_2<<endl;
    cout<<coincidence[i].source_ID_2<<endl;
    cout<<coincidence[i].position_source_2.x<<endl;
    cout<<coincidence[i].position_source_2.y<<endl;
    cout<<coincidence[i].position_source_2.z<<endl;
    cout<<coincidence[i].time_stamp_source_2<<endl;
    cout<<coincidence[i].deposited_energy_2<<endl;
    cout<<coincidence[i].position_2.x<<endl;
    cout<<coincidence[i].position_2.y<<endl;
    cout<<coincidence[i].position_2.z<<endl;
    cout<<coincidence[i].single_2.base<<endl;
    cout<<coincidence[i].single_2.rsector<<endl;
    cout<<coincidence[i].single_2.module<<endl;
    cout<<coincidence[i].single_2.submodule<<endl;
    cout<<coincidence[i].single_2.crystal<<endl;
    cout<<coincidence[i].single_2.layer<<endl;
    cout<<coincidence[i].Compton_phantom_2<<endl;
    cout<<coincidence[i].Compton_detectors_2<<endl;
    cout<<coincidence[i].Rayleigh_phantom_2<<endl;
    cout<<coincidence[i].Rayleigh_detectors_2<<endl;
    cout<<coincidence[i].scanner_axial_2<<endl;
    cout<<coincidence[i].scanner_angular_2<<endl;
 
}
*/

#endif /* GateReadSort_h */
