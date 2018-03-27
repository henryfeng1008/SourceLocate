//
//  main.cpp
//  SourceLocate
//
//  Created by Hanyu Feng on 8/10/17.
//  Copyright © 2017 Hanyu Feng. All rights reserved.
//

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <ctime>
#include "SourceLocate.h"
#include <cv.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

//      read in geometry infomation
void read_in_geometry(crystal_info *info, pos *pos){
    int i, num = 32448;
    crystal_info *geometry = (crystal_info*)malloc(num * sizeof(crystal_info));
    
    ifstream file("mCT_geometry.txt",ios::in);
    for(i = 0; i < num; i++){
        file>>geometry[i].position.x;
        file>>geometry[i].position.y;
        file>>geometry[i].position.z;
        file>>geometry[i].matrix.entry[0][0];
        file>>geometry[i].matrix.entry[0][1];
        file>>geometry[i].matrix.entry[0][2];
        file>>geometry[i].matrix.entry[1][0];
        file>>geometry[i].matrix.entry[1][1];
        file>>geometry[i].matrix.entry[1][2];
        file>>geometry[i].matrix.entry[2][0];
        file>>geometry[i].matrix.entry[2][1];
        file>>geometry[i].matrix.entry[2][2];
        file>>geometry[i].other.member_1[0];
        file>>geometry[i].other.member_1[1];
        file>>geometry[i].other.member_2[2];
        file>>geometry[i].other.member_2[0];
        file>>geometry[i].other.member_2[1];
        file>>geometry[i].other.member_2[2];
        //pos[i].x = geometry[i].position.x;
        //pos[i].y = geometry[i].position.y;
        //pos[i].z = geometry[i].position.z;
        //cout<<i<<": "<<pos[i].x<<", "<<pos[i].y<<", "<<pos[i].z<<endl;
    }
    info[0] =geometry[0];
    for (i = 0; i < num; i++){
        int ring,module,crystal;
        ring = (i/169)%4;
        module =(i/169)/4;
        crystal = i%169;
        int j = 169*48*ring + 169*module +crystal;
        //cout<<i<<", "<<ring<<", "<<module<<", "<<crystal<<", "<<j<<endl;
        info[j] = geometry[i];
    }
    for (i = 0; i < num; i++){
        pos[i].x = info[i].position.x;
        pos[i].y = info[i].position.y;
        pos[i].z = info[i].position.z;
        //cout<<i<<": "<<pos[i].x<<", "<<pos[i].y<<", "<<pos[i].z<<endl;
    }
    

    file.close();
}

//      find out how many LORs are there
int get_LORs(){
    int i = 0;
    int c1,c2,w;
    FILE *file;
    file = fopen("crystal_pair_weight.csv","r");
    while (fscanf(file, "%d,%d,%d\n", &c1, &c2, &w) == 3){
        i++;
        //printf("%d,%d,%d\n", c1,c2, w);
    }
    cout<<"total LOR: "<<i<<endl;
    return i;
}

//      read in events and weights
void read_in_crystal_pair(event *events){
    int i = 0;
    FILE *file;
    file = fopen("crystal_pair_weight.csv","r");
    while (fscanf(file, "%d,%d,%d\n", &events[i].crystal_1, &events[i].crystal_2, &events[i].weight) == 3){
        //printf("%d,%d,%d\n", events[i].crystal_1, events[i].crystal_2, events[i].weight);
        i++;
    }

}

//      initialize the FOV and voxels
void init_FOV(pos center, FOV_voxel ***voxel, dimension dim,float size){
    int i, j, k;
    int total_voxel = dim.x * dim.y * dim.z;
    
    for(i = 0; i < dim.x; i++){
        for(j = 0; j < dim.y; j++){
            for(k = 0; k < dim.z; k++){
                voxel[i][j][k].position.x = center.x + (i - 0.5*(dim.x - 1)) * size;
                voxel[i][j][k].position.y = center.y + (j - 0.5*(dim.y - 1)) * size;
                voxel[i][j][k].position.z = center.z + (k - 0.5*(dim.z - 1)) * size;
                voxel[i][j][k].weight = 0;
                
                //cout<<setw(2)<<i<<","<<setw(2)<<j<<", "<<setw(2)<<k<<": "<<setw(4)<<voxel[i][j][k].position.x<<", "<<setw(4)<<voxel[i][j][k].position.y<<", "<<setw(4)<<voxel[i][j][k].position.z<<", "<<setw(3)<<voxel[i][j][k].weight<<endl;
            }
        }
    }
    cout<<"done"<<endl;
}

//      judge if the LOR penetrate the voxel
bool in_or_not(pos center, pos crystal_1, pos crystal_2, float LOR_length, float size){
    //float LOR_length = sqrt(pow((crystal_1.x  -crystal_2.x), 2) + pow((crystal_1.y-crystal_2.y), 2) + pow((crystal_1.z-crystal_2.z), 2));
    //center.x = center.y =center.z = 0;
    
    
    float center_crystal_1 = sqrt(pow((center.x-crystal_1.x), 2) + pow((center.y-crystal_1.y), 2) + pow((center.z-crystal_1.z), 2));
    float center_crystal_2 = sqrt(pow((center.x-crystal_2.x), 2) + pow((center.y-crystal_2.y), 2) + pow((center.z-crystal_2.z), 2));
    //cout<<"cp1: "<<center_crystal_1<<", "<<"cp2: "<<center_crystal_2<<", "<<"L: "<<LOR_length<<endl;
    float cosA = (pow(center_crystal_2, 2) +pow(LOR_length, 2) - pow(center_crystal_1, 2))/(2*center_crystal_2*LOR_length);
    float sinA = sqrt(1 - pow(cosA, 2));
    float center_LOR = center_crystal_2 * sinA;
    if(center_LOR<100){
      //  cout<<center_LOR<<endl;
    }
    if(center_LOR <= size/2)
        return true;
    else return false;
}

//      show current time
void show_time(){
    time_t now_time;
    now_time = time(NULL);
    cout<<now_time<<endl;

}

//      find the max weight
long find_max(FOV_voxel ***voxels, dimension dim){
    int i, j, k;
    long max;
    max = voxels[0][0][0].weight;
    
    for(i = 0; i < dim.x; i++){
        for(j = 0; j < dim.y; j++){
            for(k = 0; k < dim.z; k++){
                if (voxels[i][j][k].weight>max)max = voxels[i][j][k].weight;
            }
        }
    }
    cout<<"max: "<<max<<endl;
    return max;
}

//  Locate
void locate(FOV_voxel ***voxels, float threshold, dimension dim,pos estimated_center, pos center){
    int i, j, k;
    float total_weight = 0;
    long w = 0;
    for(i = 0; i < dim.x; i++){
        for(j = 0; j < dim.y; j++){
            for(k = 0; k < dim.z; k++){
                w+=voxels[i][j][k].weight;
                if(voxels[i][j][k].weight > threshold){
                    total_weight += voxels[i][j][k].weight;
                    estimated_center.x += voxels[i][j][k].position.x*voxels[i][j][k].weight;
                    estimated_center.y += voxels[i][j][k].position.y*voxels[i][j][k].weight;
                    estimated_center.z += voxels[i][j][k].position.z*voxels[i][j][k].weight;
                }
            }
        }
    }
    if(total_weight ==0){
        cout<<"failed"<<endl;
    }
    else{
        estimated_center.x = estimated_center.x/total_weight;
        estimated_center.y = estimated_center.y/total_weight;
        estimated_center.z = estimated_center.z/total_weight;
        cout<<"total weight: "<<w<<endl;
        cout<<"ideal center: "<<center.x<<", "<<center.y<<", "<<center.z<<endl;
        cout<<"estimated center: "<<estimated_center.x<<", "<<estimated_center.y<<", "<<estimated_center.z<<endl;

    }
    
}

void find_intersect(pos center, pos crystal_1, pos crystal_2,long weight,FOV_voxel*** voxels, dimension dim, float size){
    int x,y,z;
    int current_x = INFINITY, current_y = INFINITY, current_z = INFINITY;
    float base_x, base_y, base_z, k,step = size/10;
    pos min,max;
    int length = 0;
    //      for x
    if (crystal_1.x<crystal_2.x){
        min.x = crystal_1.x;
        max.x = crystal_2.x;
    }
    else{
        min.x = crystal_2.x;
        max.x = crystal_1.x;
    }
    //      for y
    if (crystal_1.y<crystal_2.y){
        min.y = crystal_1.y;
        max.y = crystal_2.y;
    }
    else{
        min.y = crystal_2.y;
        max.y = crystal_1.y;
    }
    //      for z
    if (crystal_1.z<crystal_2.z){
        min.z = crystal_1.z;
        max.z = crystal_2.z;
    }
    else{
        min.z = crystal_2.z;
        max.z = crystal_1.z;
    }

    float cx = fabs(crystal_2.x - crystal_1.x);
    float cy = fabs(crystal_2.y - crystal_1.y);
    float cz = fabs(crystal_2.z - crystal_1.z);
    
    bool flag_x = false, flag_y = false, flag_z = false;
    
    if(cx>=cy && cx>=cz) flag_x = true;
    else if(cy>=cx && cy>=cz) flag_y = true;
    else flag_z = true;

    
    if (flag_z){
        //cout<<"z dir"<<endl;
        float range_1 = center.z - dim.z*0.5*size;
        float range_2 = center.z + dim.z*0.5*size;
        for(base_z = range_1; base_z <= range_2; base_z += step){
            length++;
            k = (base_z - crystal_1.z)/(crystal_2.z - crystal_1.z);
            base_x = crystal_1.x + k*(crystal_2.x - crystal_1.x);
            base_y = crystal_1.y + k*(crystal_2.y - crystal_1.y);
            
            if(base_x<=max.x && base_x>=min.y && base_y<=max.y && base_y>=min.y && base_z<=max.z && base_z>=min.z){
                x = (dim.x-1)*0.5 + round((base_x - center.x)/size);
                y = (dim.y-1)*0.5 + round((base_y - center.y)/size);
                z = (dim.z-1)*0.5 + round((base_z - center.z)/size);
                
                if(x>=0 && x<dim.x && y>=0 && y<dim.y && z>=0 && z<dim.z){
                    
                    if(current_x != x || current_y != y ||current_z != z){
                        current_x = x;
                        current_y = y;
                        current_z = z;
                        voxels[current_x][current_y][current_z].weight+= weight;
                        //cout<<current_x<<", "<<current_y<<", "<<current_z<<": "<<endl;
                        //cout<<base_x<<", "<<base_y<<", "<<base_z<<endl;
                    }
                }
                //cout<<base_x<<", "<<base_y<<", "<<base_z<<endl;
            }
        }
    }
    else if(flag_y){
        //cout<<"y dir"<<endl;
        float range_1 = center.y - dim.y*0.5*size;
        float range_2 = center.y + dim.y*0.5*size;
        for(base_y = range_1; base_y <= range_2; base_y += step){
            
            k = (base_y - crystal_1.y)/(crystal_2.y - crystal_1.y);
            base_x = crystal_1.x + k*(crystal_2.x - crystal_1.x);
            base_z = crystal_1.z + k*(crystal_2.z - crystal_1.z);
            
            if(base_x<=max.x && base_x>=min.y && base_y<=max.y && base_y>=min.y && base_z<=max.z && base_z>=min.z){
                x = (dim.x-1)*0.5 + round((base_x - center.x)/size);
                y = (dim.y-1)*0.5 + round((base_y - center.y)/size);
                z = (dim.z-1)*0.5 + round((base_z - center.z)/size);
                
                if(x>=0 && x<dim.x && y>=0 && y<dim.y && z>=0 && z<dim.z){
                    if(current_x != x || current_y != y ||current_z != z){
                        current_x = x;
                        current_y = y;
                        current_z = z;
                        voxels[current_x][current_y][current_z].weight+= weight;
                        //cout<<current_x<<", "<<current_y<<", "<<current_z<<": "<<endl;
                        //cout<<base_x<<", "<<base_y<<", "<<base_z<<endl;
                        
                    }
                }
                // cout<<base_x<<", "<<base_y<<", "<<base_z<<endl;
            }
        }
    }
    else if(flag_x){
        //cout<<"x dir"<<endl;
        float range_1 = center.x - dim.x*0.5*size;
        float range_2 = center.x + dim.x*0.5*size;
        for(base_x = range_1; base_x <= range_2; base_x += step){
            
            k = (base_x - crystal_1.x)/(crystal_2.x - crystal_1.x);
            base_z = crystal_1.z + k*(crystal_2.z - crystal_1.z);
            base_y = crystal_1.y + k*(crystal_2.y - crystal_1.y);
            
            if(base_x<=max.x && base_x>=min.y && base_y<=max.y && base_y>=min.y && base_z<=max.z && base_z>=min.z){
                x = (dim.x-1)*0.5 + round((base_x - center.x)/size);
                y = (dim.y-1)*0.5 + round((base_y - center.y)/size);
                z = (dim.z-1)*0.5 + round((base_z - center.z)/size);
                
                if(x>=0 && x<dim.x && y>=0 && y<dim.y && z>=0 && z<dim.z){
                    if(current_x != x || current_y != y ||current_z != z){
                        current_x = x;
                        current_y = y;
                        current_z = z;
                        voxels[current_x][current_y][current_z].weight+= weight;
                        //cout<<current_x<<", "<<current_y<<", "<<current_z<<": "<<weight<<endl;
                        // cout<<base_x<<", "<<base_y<<", "<<base_z<<endl;
                        
                    }
                }
                //cout<<base_x<<", "<<base_y<<", "<<base_z<<endl;
            }
        }
    }
    else cout<<"error"<<endl;
}

void generate_image(FOV_voxel ***voxels, dimension dim){
    int i, j, center = (dim.z-1)/2;
    double image[dim.x][dim.y];
    //int **image=(int**)malloc(dim.x*sizeof(int*));
    // for(i=0;i<dim.x;i++){
    //   image[i] = (int*)malloc(dim.y*sizeof(int));
    // }
    
    
    double max = voxels[0][0][center].weight;
    for(i=0;i<dim.x;i++){
        for(j=0;j<dim.y;j++){
            if (voxels[i][j][center].weight > max)
                max = voxels[i][j][center].weight;
            image[i][j] = voxels[i][j][center].weight;
        }
    }
    for(i=0;i<dim.x;i++){
        for(j=0;j<dim.y;j++){
            image[i][j] = image[i][j]/max*255;
            //cout<<i<<", "<<j<<": "<<image[i][j]<<endl;
        }
    }
    
    IplImage *img = cvCreateImage(cvSize(dim.x,dim.y), 8, 1);
    for (i=0; i<dim.x; i++)
    {
        for (j=0; j<dim.y; j++)
        {
            cvSetReal2D(img, dim.x-1-i, j, image[j][i]);
        }
    }
    cvNamedWindow("copy", 1);
    cvShowImage("copy", img);
    cvWaitKey();
    cvSaveImage("image.jpg",img);
}



int main(int argc, const char * argv[]) {
    int num = 32448, i, j;
    int LORs = get_LORs();
    dimension dim = {401, 401, 201};
    
    float size =0.01;
    pos center = {0, 400, 0};
    
    crystal_info *info = (crystal_info*)malloc(num * sizeof(crystal_info));
    
    pos *positions = (pos*)malloc(num * sizeof(pos));
    
    event *events = (event*)malloc(LORs*sizeof(event));
    
    FOV_voxel ***voxels = (FOV_voxel***)malloc(dim.x*sizeof(FOV_voxel**));
    for(i = 0; i < dim.x; i++){
        voxels[i] =(FOV_voxel**)malloc(dim.y*sizeof(FOV_voxel*));
        for(j = 0; j < dim.y; j++){
            voxels[i][j] =(FOV_voxel*)malloc(dim.z*sizeof(FOV_voxel));
        }
    }

    // initialize the voxel in FOV
    init_FOV(center, voxels, dim, size);
    cout<<"finished initialize the FOV"<<endl;
    
    //      read in geometry infomation
    read_in_geometry(info, positions);
    cout<<"finished reading geometry"<<endl;
    
    //      read in every event and counts
    read_in_crystal_pair(events);
    cout<<"finished loding the crystal pairs and weights"<<endl;
    
    
    //cout<<"true:"<<true<<endl;
    int report = 1;
    //      loop all the LORs
    for(i = 0; i < LORs; i++){
       // cout<<"LOR: "<<i<<endl;
        find_intersect(center, positions[events[i].crystal_1], positions[events[i].crystal_2], events[i].weight,  voxels, dim, size);
        if(i > report*20000){
            //show_time();
            cout<<"No: "<<i<<" LOR finished"<<endl;
            report++;
        }

    }
    
    long max = find_max(voxels, dim);
    float threadshold = max * 0.7;
    pos estimated_center = {0, 0, 0};
    locate(voxels, threadshold, dim, estimated_center, center);
    generate_image(voxels, dim);
    free(voxels);
    return 0;
}



/*
for(j = 0; j < total_voxel; j++){
    
    //cout<<in_or_not(voxels[j].position, positions[events[i].crystal_1], positions[events[i].crystal_2], LOR_length)<<endl;
    
     if(in_or_not(voxels[j].position, positions[events[i].crystal_1], positions[events[i].crystal_2], LOR_length, size)){
     voxels[j].weight +=events[i].weight; //voxels[j].weight + 1;//
     valid++;
     }
}
 
 
 
 float LOR_length = sqrt(pow((positions[events[i].crystal_1].x-positions[events[i].crystal_2].x), 2) + pow((positions[events[i].crystal_1].y-positions[events[i].crystal_2].y), 2) + pow((positions[events[i].crystal_1].z-positions[events[i].crystal_2].z), 2));
 //if(in_or_not(center, positions[events[i].crystal_1], positions[events[i].crystal_2], LOR_length, size[0]))
 for(j = 0; j < total_voxel; j++){
 
 //cout<<in_or_not(voxels[j].position, positions[events[i].crystal_1], positions[events[i].crystal_2], LOR_length)<<endl;
 
 if(in_or_not(voxels[j].position, positions[events[i].crystal_1], positions[events[i].crystal_2], LOR_length, size)){
 voxels[j].weight +=events[i].weight; //voxels[j].weight + 1;//
 valid++;
 }
 }
 //cout<<"No: "<<i<<" LOR finished"<<endl;
 //      loop all the voxels for each LOR
 if(i > report*20000){
 //show_time();
 cout<<"No: "<<i<<" LOR finished"<<endl;
 report++;
 }
*/
