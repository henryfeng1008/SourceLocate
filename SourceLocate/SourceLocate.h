//
//  SourceLocate.h
//  SourceLocate
//
//  Created by Hanyu Feng on 8/10/17.
//  Copyright © 2017 Hanyu Feng. All rights reserved.
//

#ifndef SourceLocate_h
#define SourceLocate_h
using namespace std;

typedef struct{
    float x;
    float y;
    float z;
}pos;

typedef struct{
    int x;
    int y;
    int z;
}dimension;

typedef struct{
    float entry[3][3];
}rotation_matrix;

typedef struct{
    float member_1[3];
    float member_2[3];
}other_item;

typedef struct{
    pos position;
    rotation_matrix matrix;
    other_item other;
}crystal_info;


typedef struct{
    pos position;
}crystal_pos;

typedef struct{
    int crystal_1;
    int crystal_2;
    int weight;
}event;

typedef struct{
    pos position;
    double weight;
}FOV_voxel;

#endif /* SourceLocate_h */
