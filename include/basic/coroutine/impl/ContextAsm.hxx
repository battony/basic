#pragma once

extern "C" void MakeContext(void** ptr, void (*fun)());
extern "C" void SwitchContext(void** old_ptr, void** new_ptr);
