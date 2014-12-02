//
//  InitOnce.h
//  Thermocouple
//
//  Created by Jeff Kilpatrick on 11/23/14.
//  Copyright (c) 2014 Jeff Kilpatrick. All rights reserved.
//

#ifndef Thermocouple_InitOnce_h
#define Thermocouple_InitOnce_h

void InitializeStaticRepository();

void DeinitializeStaticRepository();

template<typename T, typename F>
void InitOnce(T* obj, F fn);

#endif
