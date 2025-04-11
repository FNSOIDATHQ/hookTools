#include "hookTool.h"

//reference:
//https://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
//https://ocw.cs.pub.ro/courses/so/laboratoare/resurse/injections

//armada is 32bit so this is enough for now
//simple JMP will break original function
void hookJMP(void* oriFunc, void* replFunc){
    DWORD oldProtectValue;
    //func(StartAddress,size,protectStatus,oldStatus)
    //i am not pretty sure, but 5 not 1024 is completely enough
    VirtualProtect(oriFunc, 1024, PAGE_EXECUTE_READWRITE, &oldProtectValue);

    //32 bit relative jump opcode is E9, takes 1 32 bit operand for jump offset
    uint8_t jmpInstruction[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };

    //to fill out the last 4 bytes of jmpInstruction, we need the offset between
    //the payload function and the instruction immediately AFTER the jmp instruction
    const uint32_t relativeAddr = (uint32_t)replFunc - ((uint32_t)oriFunc + sizeof(jmpInstruction));
    memcpy(jmpInstruction + 1, &relativeAddr, 4);

    //install the hook
    memcpy(oriFunc, jmpInstruction, sizeof(jmpInstruction));

    VirtualProtect(oriFunc, 1024, oldProtectValue, &oldProtectValue);
}

//TODO:
//run our own code and back to original
void hookTrampoline(void* oriFunc, void* replFunc,void** trampolinePtr){
    DWORD oldProtectValue;
    VirtualProtect(oriFunc, 1024, PAGE_EXECUTE_READWRITE, &oldProtectValue);

    //void* hookMemory = AllocatePageNearAddress(oriFunc);
    //uint32_t trampolineSize = BuildTrampoline(oriFunc, hookMemory);
    //*trampolinePtr = hookMemory;

    //32 bit relative jump opcode is E9, takes 1 32 bit operand for jump offset
    uint8_t jmpInstruction[5] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };

    //to fill out the last 4 bytes of jmpInstruction, we need the offset between
    //the payload function and the instruction immediately AFTER the jmp instruction
    const uint32_t relativeAddr = (uint32_t)replFunc - ((uint32_t)oriFunc + sizeof(jmpInstruction));
    memcpy(jmpInstruction + 1, &relativeAddr, 4);

    //install the hook
    memcpy(oriFunc, jmpInstruction, sizeof(jmpInstruction));

    VirtualProtect(oriFunc, 1024, oldProtectValue, &oldProtectValue);
}

//reference https://www.codereversing.com/archives/596
//return original function ptr
void* hookVTable(void** vtableBaseAddress,size_t index,void* hookAddress) {
    void* originalFunc=vtableBaseAddress[index];
    DWORD oldProtectValue;
    VirtualProtect(&vtableBaseAddress[index], sizeof(intptr_t), PAGE_EXECUTE_READWRITE, &oldProtectValue);

    memcpy(&vtableBaseAddress[index], &hookAddress, sizeof(intptr_t));

    VirtualProtect(&vtableBaseAddress[index], sizeof(intptr_t), oldProtectValue, &oldProtectValue);

    return originalFunc;
}

void writeVarToAddress(UINT address,UINT dataSize,void* input){
    void* targetPointer=(void*)address;

    DWORD oldProtectValue;
    VirtualProtect(targetPointer, dataSize, PAGE_EXECUTE_READWRITE, &oldProtectValue);

    memcpy(targetPointer, input, dataSize);

    VirtualProtect(targetPointer, dataSize, oldProtectValue, &oldProtectValue);
}

void writeVarToAddressP(void* targetPointer,UINT dataSize,void* input){
    DWORD oldProtectValue;
    VirtualProtect(targetPointer, dataSize, PAGE_EXECUTE_READWRITE, &oldProtectValue);

    memcpy(targetPointer, input, dataSize);

    VirtualProtect(targetPointer, dataSize, oldProtectValue, &oldProtectValue);
}

void writeNopsToAddress(UINT address,UINT dataSize){

    unsigned char* data=new unsigned char[dataSize];
    for(UINT i=0;i<dataSize;i++){
        data[i]=0x90;
    }

    void* targetPointer=(void*)address;

    DWORD oldProtectValue;
    VirtualProtect(targetPointer, dataSize, PAGE_EXECUTE_READWRITE, &oldProtectValue);

    memcpy(targetPointer, data, dataSize);

    VirtualProtect(targetPointer, dataSize, oldProtectValue, &oldProtectValue);
}

//this function is unchecked
void* getClassFunctionAddress(DWORD* classPtr,int index){
    DWORD** classVtable=(DWORD**)*classPtr;
    //std::string msg= "classVtable"+std::to_string((int)classVtable);
    //MessageBoxA(0,msg.c_str(), "test", MB_OK | MB_ICONINFORMATION);

    //void* func=(void*)*(DWORD*)(classVtable+sizeof(classVtable)*index);
    void* func=(void*)classVtable[index];
    //msg= "func"+std::to_string((int)func);
    //MessageBoxA(0, msg.c_str(), "test", MB_OK | MB_ICONINFORMATION);

     return func;
}

__declspec(naked) UINT getThisPtrFromECX(){
    __asm{
        mov     eax, ecx
        retn
    }
}

__declspec(naked) void moveVarToECX(UINT input){
    __asm{
        lea     ecx, [eax]
        retn
    }
}
