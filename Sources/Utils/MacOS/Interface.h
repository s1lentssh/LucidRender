//
// Interface.h
//

#ifndef INTERFACE_H
#define INTERFACE_H

extern "C"
{
    void* getMetalLayer(void* NSWindow);
    void makeViewMetalCompatible(void* handle);
}

#endif /* INTERFACE_H */
