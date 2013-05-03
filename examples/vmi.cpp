#include "vmi.h"
#include <iostream>
void Vmi::Initialize(){
	if(vmi_init(&_vmi, VMI_AUTO | VMI_INIT_COMPLETE, (char*)_name.c_str())== VMI_FAILURE){
		printf("Failed to init LibVMI library.\n");
		Exit();
		return;
	}
	//TODO deal with error
	GetOsTypeAndOffsets();
	_inited = true;

}
void Vmi::GetOsTypeAndOffsets(){
	if(VMI_OS_LINUX ==vmi_get_ostype(_vmi)){
		_tasks_offset = vmi_get_offset(_vmi, "linux_tasks");
		_name_offset = vmi_get_offset(_vmi, "linux_name");
		_pid_offset = vmi_get_offset(_vmi, "linux_pid");
		_os_type = VMI_OS_LINUX;
	} 
	else if(VMI_OS_WINDOWS == vmi_get_ostype(_vmi)){
		_tasks_offset = vmi_get_offset(_vmi, "win_tasks");
		if(0==_tasks_offset){
			printf("Failed to find win_tasks\n");
			Exit();
		}

		_name_offset = vmi_get_offset(_vmi, "win_pname");
		if(0==_name_offset){
			printf("Failed to find win_pname\n");
			Exit();
		}
		_pid_offset = vmi_get_offset(_vmi, "win_pid");
		if(0==_pid_offset){
			printf("Failed to find win_pid\n");
			Exit();
		}
		_os_type = VMI_OS_WINDOWS;	
	}		
}

void Vmi::Exit(){
	if (_procname) {
		free(_procname);
		_procname = NULL;
	}
	/*resume the vm*/
	vmi_resume_vm(_vmi);
	//cleanup any memory associated with the LibVMI instance */
}
// return 0 if not found, else return pid
int Vmi::FindProcess(string processName){
	addr_t next_process, list_head;
	uint32_t  pid = 0;
	int processId = -1;
	bool find = false;
	_procname = NULL;

	if(!_inited){
		Initialize();
	}	

	Pause_vm();

	if(VMI_OS_LINUX == _os_type){
		addr_t init_task_va = vmi_translate_ksym2v(_vmi, "init_task");
		vmi_read_addr_va(_vmi, init_task_va+_tasks_offset, 0, &next_process);
	}	
	else if(VMI_OS_WINDOWS == _os_type){
		uint32_t pdbase = 0;

		// find PEPROCESS PsInitialSystemProcess
		vmi_read_addr_ksym(_vmi, "PsInitialSystemProcess", &list_head); 
		vmi_read_addr_va(_vmi, list_head+_tasks_offset, 0, &next_process);
		vmi_read_32_va(_vmi, list_head+_pid_offset, 0, &pid);	
		_procname = vmi_read_str_va(_vmi, list_head+_name_offset, 0);
		if(!_procname){
			printf("Failed to find first _procname\n");
			Exit();
		}
		if(_procname){
			free(_procname);
			_procname = NULL;
		}
	}

	list_head = next_process;

	while(!find){
		addr_t tmp_next = 0;
		vmi_read_addr_va(_vmi, next_process, 0, &tmp_next);
		if(list_head == tmp_next){
			break;
		}

		_procname = vmi_read_str_va(_vmi, next_process + _name_offset - _tasks_offset, 0);

		if (!_procname) {
			printf ("Failed to find procname\n");
		} // if

		vmi_read_32_va(_vmi, next_process + _pid_offset - _tasks_offset, 0, &pid);

		/* trivial sanity check on data */
		if (pid >= 0 && _procname){
			printf("[%5d] %s\n", pid, _procname);
		}
		if (_procname){
			if(processName.compare(_procname)==0){
				cout<<"found"<<endl;
				find = true;
				processId = pid;
			}
			free(_procname);
			_procname = NULL;
		}
		next_process = tmp_next;

	}
	cout<<"start exit"<<endl;
	Exit();
	cout<<"end exit"<<processId<<endl;

	return processId;
	
}
int main(){
	int pid;
	Vmi vm02("vm02");
	pid = vm02.FindProcess("sshd");
	cout<<"The pid found is "<<pid<<endl;
	pid = vm02.FindProcess("init");
	cout<<"The pid found is "<<pid<<endl;
	pid = vm02.FindProcess("swy");
	cout<<"The pid found is "<<pid<<endl;

}
