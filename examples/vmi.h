#ifndef _VMI_CLASS_H
#define _VMI_CLASS_H

#include <libvmi/libvmi.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>

using namespace::std;

class Vmi{
	//member variables
	vmi_instance_t _vmi;
	int _os_type;
	int _tasks_offset, _pid_offset, _name_offset;
	string _name;//virtual machine name
	char* _procname;

	bool _inited;// whether it's inited
	public:
	 Vmi(string vm_name){
		_name = vm_name;
		_inited = false;
		_vmi = NULL;
	}
	~Vmi(){
		vmi_destroy(_vmi);
	}
	private: void Initialize();
	private: void GetOsTypeAndOffsets();

	private: void Exit();

	void Pause_vm(){
		if(vmi_pause_vm(_vmi) != VMI_SUCCESS){
			printf("Failed to pause VM\n");
			Exit();
		}		
	}
	void Resume_vm(){
		vmi_resume_vm(_vmi);
	}
	void Dump_memory(string fileName){

	}
	// return 0 if not found, else return pid
	public: int FindProcess(string processName);
};
#endif
