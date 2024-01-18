#include "task.h"
#include "common.h"
#include "memory.h"

volatile Task::Task* currentTask;
volatile Task::Task* readyQueue;

extern Memory::PageDirectory* kernelDirectory;
extern Memory::PageDirectory* currentDirectory;
extern "C" {
	u32 readEIP();
}

u32 nextPID = 1;

void Task::moveStack(void* newStart, u32 size) {
	assert(newStart != nullptr, "Task::moveStack: new start is null");
	for (u32 i = (u32)newStart; i >= ((u32)newStart-size); i -= 0x1000) {
		currentDirectory->getPage(i, true)->alloc(false, true);
	}

	u32 pageDirectoryAddress;
	asm volatile("mov %%cr3, %0" : "=r" (pageDirectoryAddress));
	asm volatile("mov %0, %%cr3" : : "r" (pageDirectoryAddress));

	u32 oldStackPointer;
	u32 oldBasePointer;
	asm volatile("mov %%esp, %0" : "=r" (oldStackPointer));
	asm volatile("mov %%ebp, %0" : "=r" (oldBasePointer));

	u32 offset = (u32)newStart - initialEsp;
	u32 newStackPointer = oldStackPointer + offset;
	u32 newBasePointer = oldBasePointer + offset;
	memcpy((const void*)newStackPointer, (void*)oldStackPointer, initialEsp - oldStackPointer);
	for (u32 i = (u32)newStart; i > (u32)newStart - size; i -= 4) {
		u32 tmp = *(u32*)i;
		if ((oldStackPointer < tmp) && (tmp < initialEsp)) {
			tmp += offset;
			u32* tmp2 = (u32*)i;
			*tmp2 = tmp;
		}
	}
	asm volatile("mov %%esp, %0" : "=r" (newStackPointer));
	asm volatile("mov %%ebp, %0" : "=r" (newBasePointer));

}

void Task::init() {
	asm volatile("cli");
	moveStack((void*)0xE0000000, 0x2000);
	currentTask = readyQueue = Memory::kmalloc<Task>();
	currentTask->id = nextPID++;
	currentTask->esp = currentTask->ebp = 0;
	currentTask->eip = 0;
	currentTask->pageDirectory = currentDirectory;
	currentTask->next = nullptr;
	asm volatile("sti");
}

u32 Task::fork() {
	asm volatile("cli");

	Task* parent = (Task*)currentTask;
	Memory::PageDirectory* dir = currentDirectory->clone();

	Task* newTask = Memory::kmalloc<Task>();
	newTask->id = nextPID++;
	newTask->esp = newTask->ebp = 0;
	newTask->eip = 0;
	newTask->pageDirectory = dir;
	newTask->next = nullptr;

	Task* tmp = (Task*)readyQueue;
	while (tmp->next) {
		tmp = tmp->next;
	}
	tmp->next = newTask;
	u32 eip = readEIP();

	if (currentTask == parent) {
		u32 esp;
		u32 ebp;
		asm volatile("mov %%esp, %0" : "=r"(esp));
		asm volatile("mov %%ebp, %0" : "=r"(ebp));
		newTask->esp = esp;
		newTask->ebp = ebp;
		newTask->eip = eip;
		asm volatile("sti");
		return newTask->id;
	}
	return 0;
}

void Task::switchTask() {
	if (!currentTask) {
		return;
	}
	u32 esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));

	eip = readEIP();

	if (eip == 0x12345) {
		return;
	}

	// No task switch
	currentTask->eip = eip;
	currentTask->esp = esp;
	currentTask->ebp = ebp;

	currentTask = currentTask->next;
	if (!currentTask) {
		currentTask = readyQueue;
	}

	esp = currentTask->esp;
	ebp = currentTask->ebp;

	asm volatile("cli; mov %0, %%ecx;  mov %1, %%esp; mov %2, %%ebp; mov %3, %%cr3; mov $0x12345, %%eax; sti; jmp *%%ecx" : : "r"(eip), "r"(esp), "r"(ebp), "r"(currentDirectory->physicalAddr));
}
