//
//  main.cpp
//  GateReadSort
//
//  Created by Hanyu Feng on 8/7/17.
//  Copyright © 2017 Hanyu Feng. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <string.h>
#include <math.h>

#include "GateReadSort.h"

using namespace std;

/*void creat_csv(){
    ofstream file;
    file.open("train_LOR_0_01_0.csv",ios::in);
    if(!file){
        file<<"LOR"<<","<<"repeat"<<endl;
        file.close();
    }
    
}*/

//      reverse calculate the crystal pair from LOR
void LOR_to_pair(long *LOR_with_time, int i, int *crystal_1, int *crystal_2){
    crystal_1[i]= 0;
    
    while((LOR_with_time[i] - crystal_1[i]*32448 + 0.5*crystal_1[i]*(1 + crystal_1[i]) + crystal_1[i]) > 32447){
        crystal_1[i]++;
    }
    crystal_2[i] = LOR_with_time[i] - crystal_1[i]*32448 + 0.5*crystal_1[i]*(1 + crystal_1[i]) + crystal_1[i];
}



//      write the sorted LOR with repeat
void write_sorted_csv(long* event, long total_event){
    int i;
    ofstream file;
    file.open("sorted_LOR.csv",ios::out);
    for(i=0;i<total_event;i++){
        file<<event[i]<<endl;
    }
    file.close();
}

//      write the sorted LOR without repeat
void write_repeat_csv(long* LOR,int *repeat, int count){
    int i;
    ofstream file;
    file.open("repeat_LOR.csv",ios::out);
    for(i = 0; i < count; i++){
        file<<LOR[i]<<","<<repeat[i]<<endl;
    }
    file.close();
}

//      write the crystal pair of each event with repeat
void write_crystal_pair_sorted(long *LOR, int *repeat, int count){
    int i;
    int *crystal_1 = (int*)malloc(count*sizeof(int));
    int *crystal_2 = (int*)malloc(count*sizeof(int));
    ofstream file;
    file.open("crystal_pair_weight.csv",ios::out);
    for(i = 0; i < count; i++){
        LOR_to_pair(LOR, i, crystal_1, crystal_2);
        //cout<<crystal_1[i]<<","<<crystal_2[i]<<endl;
        file<<crystal_1[i]<<","<<crystal_2[i]<<","<<repeat[i]<<endl;
    }
    file.close();

}

//      write the crystal pair in binary format
void write_crystal_pair(Event *LOR, long total_event){
    int i;
    ofstream file;
    file.open("crystal_pair.csv",ios::out);
    for(i = 0; i < total_event; i++){
        file<<LOR[i].crystal_1<<","<<LOR[i].crystal_2<<endl;
    }
    file.close();
    
}





//      calculate the LOR from crystal pair
void get_LOR(Event *LOR_time, coincidence_struct *coincidence, int i){
    int crystal_1 =169*coincidence[i].single_1.rsector + 169*48*coincidence[i].single_1.module + coincidence[i].single_1.crystal;
    int crystal_2 =169*coincidence[i].single_2.rsector + 169*48*coincidence[i].single_2.module + coincidence[i].single_2.crystal;
    long LOR;
    if(crystal_1 < crystal_2){
        LOR_time[i].crystal_1 = crystal_1;
        LOR_time[i].crystal_2 = crystal_2;
        LOR = 32448*crystal_1 - (1+crystal_1)*crystal_1/2 + crystal_2-crystal_1;
    }
    else {
        LOR_time[i].crystal_1 = crystal_2;
        LOR_time[i].crystal_2 = crystal_1;
        LOR = 32448*crystal_2 - (1+crystal_2)*crystal_2/2 + crystal_1-crystal_2;
    }
    LOR_time[i].LOR = LOR;
    LOR_time[i].time = coincidence[i].time_stamp_source_1;
    
}

//      count the number of different LOR
int get_num_LOR(long *event, long total_event){
    int i;
    int count = 1;
    for (i = 1; i < total_event; i++){
        if(event[i]!=event[i-1]){
            count++;
        }
    }
    return count;
}

//      count the repeat time of each LOR
void count_LOR(long *event, long *LOR, int *repeat, long total_event, int count){
    int i,j = 1;
    LOR[0] = event[0];
    repeat[0] = 1;
    
    for (i = 1; i < total_event; i++){
        if(event[i] != event[i-1]){
            LOR[j] = event[i];
            repeat[j] = 1;
            j++;
        }
        else{
            repeat[j-1]++;
        }
    }
}


int main(int argc, const char * argv[]) {
    int i;
    long total_event, length;
    double total_time, time_seg, segment;
    char *filename = "b0_400_0_90sCoincidences.dat";
    //  find the number of event
    ifstream file1;
    file1.open(filename,ios::binary);
    file1.seekg(0, ios::end);
    length = file1.tellg();
    file1.seekg(0,ios::beg);
    file1.close();
    total_event = length / sizeof(coincidence_struct);
    
    cout<<"total event: "<<total_event<<endl;

    //  read in every coincidence
    coincidence_struct *coincidence = (coincidence_struct*)malloc(total_event*sizeof(coincidence_struct));
    Event *LOR_with_time = (Event*)malloc(total_event*sizeof(Event));
    FILE *file = fopen(filename, "rb");
    fread(coincidence,sizeof(coincidence_struct),total_event,file);
    
    //  calculate the order of the LOR of each event
    for(i = 0; i < total_event; i++){
        get_LOR(LOR_with_time, coincidence, i);
    }
    
    long *event = (long*)malloc(total_event*sizeof(long));
    for(i = 0; i < total_event; i++){
        event[i] = LOR_with_time[i].LOR;
    }
    sort(event,event+total_event);
    
    
    
    int count = get_num_LOR(event, total_event);
    
    long *LOR = (long*)malloc(count*sizeof(long));
    int *repeat = (int*)malloc(count*sizeof(int));

    count_LOR(event, LOR, repeat, total_event, count);
    cout<<"non-repeat: "<<count<<endl;
    //write_sorted_csv(event, total_event);
    //write_repeat_csv(LOR, repeat, count);
    write_crystal_pair_sorted(LOR, repeat, count);
    //write_crystal_pair(LOR_with_time, total_event);
    return 0;
}
