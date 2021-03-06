#include "KernelData.h"


int compileKernel__kernelData(KernelData *self, const char *srcKernel, const char *functionOpencl){
    cl_int catchCodeCode;
     if(self->kernel){
        catchCodeCode = clReleaseKernel(self->kernel);checkCL(catchCodeCode);
    }
    if(self->program){
        catchCodeCode = clReleaseProgram(self->program);checkCL(catchCodeCode);
    }
    self->program=0x0;
    self->kernel=0x0;
    
    //Create a program from the kernel source===================================================
    self->program = clCreateProgramWithSource(self->context, 1, (const char **)&srcKernel,NULL, &catchCodeCode);
    checkCL(catchCodeCode);
     //End=======================================================================================

    //Build the program==========================================================================
    

    catchCodeCode = clBuildProgram(self->program, 1, &self->device_id, NULL, NULL, NULL);
    size_t logSize=0;
    clGetProgramBuildInfo(self->program, self->device_id,CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
    if(catchCodeCode !=CL_SUCCESS && logSize >0){
        clGetProgramBuildInfo(self->program, self->device_id,CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
    
        char *srcLog=(char *)malloc(sizeof(char)*logSize);
        clGetProgramBuildInfo(self->program, self->device_id, CL_PROGRAM_BUILD_LOG, logSize, srcLog, NULL);
        printf("*********************Error in opencl Kernel*********************\n\n");
        printf("%s \n", srcLog);
        printf("******************************END*******************************\n");
    }
    checkCL(catchCodeCode);
    //End========================================================================================
    
    //Create the OpenCL kernel===================================================================
    self->kernel = clCreateKernel(self->program, functionOpencl, &catchCodeCode);
    checkCL(catchCodeCode);
    //End=========================================================================n===============
    return catchCodeCode;
}

int auxForConstructor__KernelData(KernelData *self,cl_platform_id platforms, cl_device_id device_id){
    self->context=NULL;
    self->device_id=device_id;
    self->command_queue=NULL;
    self->program=NULL;
    self->kernel=NULL;
    cl_int catchCode;
    
    //Create an OpenCL context==================================================================
    self->context= clCreateContext( NULL, 1, &device_id, NULL, NULL, &catchCode);
    checkCL(catchCode);
    //End=======================================================================================

    //Create a command queue====================================================================
    self->command_queue = clCreateCommandQueue(self->context, device_id, 0, &catchCode);    
    checkCL(catchCode);
    //End=======================================================================================
    
    self->kernelCreatedFromAnotherKernel=0;
    return catchCode;
}

cl_device_id getDevice_id__kernelData(KernelData *self){
    return self->device_id;
}

cl_context getContext__KernelData(KernelData *self){
    return self->context;
}

cl_kernel  getKernel__KernelData(KernelData *self){
    return self->kernel;
}
cl_program getProgram__KernelData(KernelData *self){
    return self->program;
}

cl_command_queue getCommand_queue__KernelData(KernelData *self){
    return self->command_queue;
}

KernelData *new_5__KernelData(char *srcKernel, const char* functionOpencl, cl_platform_id platform, cl_int *catchCode) {
    KernelData *self=(KernelData *)malloc(sizeof(KernelData)); checkMallocReturnPointer(self,catchCode);
	self->program = NULL;
	self->kernel = NULL;
	size_t source_size = strlen(srcKernel);
	*catchCode = 0;


	// Create the properties for this context.
	cl_context_properties lContextProperties[] = {
		// We need to add information about the OpenGL context with
		// which we want to exchange information with the OpenCL context.
#if defined (WIN32)
		// We should first check for cl_khr_gl_sharing extension.
		CL_GL_CONTEXT_KHR , (cl_context_properties)wglGetCurrentContext() ,
		CL_WGL_HDC_KHR , (cl_context_properties)wglGetCurrentDC() ,
#elif defined (__linux__)
		// We should first check for cl_khr_gl_sharing extension.
		CL_GL_CONTEXT_KHR , (cl_context_properties)glXGetCurrentContext() ,
		CL_GLX_DISPLAY_KHR , (cl_context_properties)glXGetCurrentDisplay() ,
#elif defined (__APPLE__)
		// We should first check for cl_APPLE_gl_sharing extension.
#if 0
		// This doesn't work.
		CL_GL_CONTEXT_KHR , (cl_context_properties)CGLGetCurrentContext() ,
		CL_CGL_SHAREGROUP_KHR , (cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()) ,
#else
		CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE , (cl_context_properties)CGLGetShareGroup(CGLGetCurrentContext()) ,
#endif
#endif
		CL_CONTEXT_PLATFORM , (cl_context_properties)platform ,
		0 
	};


	size_t catchSize;
	clGetGLContextInfoKHR_fn func = (clGetGLContextInfoKHR_fn) clGetExtensionFunctionAddressForPlatform(platform,"clGetGLContextInfoKHR");
	if (func == NULL) {
		*catchCode = CL_Errors_TheExtensionDoesNotExistForThisPlatform;
		checkCLReturnValue(*catchCode,self);
	}
	*catchCode= func(lContextProperties, CL_CURRENT_DEVICE_FOR_GL_CONTEXT_KHR,sizeof(cl_device_id), &self->device_id, &catchSize);
	checkCLReturnValue(*catchCode,self);
	if (catchSize == 0) {
		*catchCode = CL_Errors_NoAssociatedDeviceWithOpenGLForThisPlatform;
		checkCLReturnValue(*catchCode,self);
	}

	
	self->context = clCreateContext(lContextProperties,1, &self->device_id, NULL, 0,  catchCode);
	checkCLReturnValue(*catchCode,self);
	self->command_queue = clCreateCommandQueue(self->context, self->device_id, 0, catchCode);
	checkCLReturnValue(*catchCode,self);    
	*catchCode = compileKernel__kernelData(self,srcKernel, functionOpencl); checkCLReturnValue(*catchCode,self);
	checkCLReturnValue(*catchCode,self); 
	self->kernelCreatedFromAnotherKernel = 0;
    return self;
}

KernelData* new_4__KernelData(cl_context context, cl_command_queue command_queue,cl_device_id device_id,char *srcKernel, const char *functionOpencl, cl_int *catchCode){
    KernelData *self=(KernelData *)malloc(sizeof(KernelData)); checkMallocReturnPointer(self,catchCode);
    self->context=context;
    self->command_queue=command_queue;
    self->program=NULL;
    self->kernel=NULL;
    self->device_id=device_id;
    *catchCode=compileKernel__kernelData(self,srcKernel,functionOpencl); checkCLReturnValue(*catchCode,self);
    self->kernelCreatedFromAnotherKernel=1;
    return self;
}

KernelData* new_3__KernelData(cl_context context, cl_command_queue command_queue, cl_device_id device_id, int *catchCode){
    KernelData *self=(KernelData *)malloc(sizeof(KernelData)); checkMallocReturnPointer(self,catchCode);
    self->device_id=device_id;
    self->context=context;
    self->command_queue=command_queue;
    self->program=NULL;
    self->kernel=NULL;
    self->kernelCreatedFromAnotherKernel=1;
    return self;      
}



KernelData* new_2__KernelData(cl_platform_id platforms, cl_device_id device_id, cl_int *catchCode){
    KernelData *self=(KernelData *)malloc(sizeof(KernelData)); checkMallocReturnPointer(self,catchCode);
    *catchCode=auxForConstructor__KernelData(self,platforms,device_id); checkCLReturnValue(*catchCode,self);
    return self;
}

KernelData* new__KernelData(char *srcKernel,const char* functionOpencl, cl_platform_id platforms, cl_device_id device_id, cl_int *catchCode){
    KernelData *self=(KernelData *)malloc(sizeof(KernelData)); checkMallocReturnPointer(self,catchCode);
    size_t source_size=strlen(srcKernel); 
    *catchCode=auxForConstructor__KernelData(self,platforms,device_id); checkCLReturnValue(*catchCode,self);
    *catchCode=compileKernel__kernelData(self,srcKernel,functionOpencl); checkCLReturnValue(*catchCode,self);    
    self->kernelCreatedFromAnotherKernel=0;
    return self;
}

int release__KernelData(KernelData *self){
    cl_int catchCode=CL_SUCCESS;
    if(self->kernel){
        catchCode = clReleaseKernel(self->kernel);
        
    }
    if(self->program){
        catchCode = clReleaseProgram(self->program);
        
    }
    if(self->command_queue && !self->kernelCreatedFromAnotherKernel){
        catchCode = clReleaseCommandQueue(self->command_queue);
    }
    if(self->context && !self->kernelCreatedFromAnotherKernel){
        catchCode = clReleaseContext(self->context);       
    }
    free(self);
    return catchCode;
}
