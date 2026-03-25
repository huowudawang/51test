#include <REGX52.H>

#define MAX_TASKS 2       // 最大任务数量
#define MAX_TASK_DEP 32    // 每个任务的栈深度
#define RUNNING 0         // 任务状态：运行
#define SUSPEND 1         // 任务状态：挂起

// --- 全局变量 ---
unsigned char idata task_sp[MAX_TASKS];              // 存储各任务的栈指针(SP)
unsigned char idata task_stack[MAX_TASKS][MAX_TASK_DEP]; // 各任务独立的栈空间
unsigned int  idata task_timer[MAX_TASKS];           // 任务延时计数器（单位：10ms）
unsigned char idata task_state[MAX_TASKS];           // 任务状态（运行/挂起）
unsigned char task_id;                               // 当前正在运行的任务ID

// --- 函数声明 ---
void task_switch(void);
void task_load(unsigned int fn, unsigned char tid);
void Task_Delay(unsigned int ms);
void Timer0_Init(void);

// --- 任务0：0.5秒闪烁 (500ms) ---
void task0() {
    P1_0 = 0;                     // 初始化灯状态
    while(1) {
        P1_0 = ~P1_0;             // 翻转LED电平
        Task_Delay(500);          // 调用系统延时，挂起500ms
    }
}

// --- 任务1：1秒闪烁 (1000ms) ---
void task1() {
    P1_7 = 0;                     // 初始化灯状态
    while(1) {
        P1_7 = ~P1_7;             // 翻转LED电平
        Task_Delay(1000);         // 调用系统延时，挂起1000ms
    }
}

void main() {
    Timer0_Init();                // 第一步：配置定时器产生系统滴答
    task_load((unsigned int)task0, 0); // 第二步：装载任务0
    task_load((unsigned int)task1, 1); // 第三步：装载任务1
    
    task_id = 0;                  // 从任务0开始运行
    SP = task_sp[0];              // 手动切换堆栈指针到任务0
    
    // 技巧：利用汇编回到任务入口地址执行
    // 在C语言中，SP指向处已经是task0地址，直接执行即可
    task0(); 
}

// --- 任务装载函数：初始化任务的栈，伪造一个现场 ---
void task_load(unsigned int fn, unsigned char tid) {
    task_stack[tid][0] = (unsigned char)fn;         // 在栈底存入函数地址低8位
    task_stack[tid][1] = (unsigned char)(fn >> 8);  // 在栈底存入函数地址高8位
    task_sp[tid] = (unsigned char)&task_stack[tid][1]; // 记录当前栈顶位置
    task_state[tid] = RUNNING;                      // 初始状态设为运行
    task_timer[tid] = 0;                            // 初始延时为0
}

// --- 系统延时函数：非阻塞 ---
void Task_Delay(unsigned int ms) {
    task_timer[task_id] = ms / 10; // 将毫秒换算成10ms的滴答数
    task_switch();                 // 设置完延时后，立即切换到其他任务，不浪费CPU
}

// --- 调度器：选择下一个可运行的任务 ---
void task_switch() {
    task_sp[task_id] = SP;         // 1. 保存当前任务的栈指针到数组
    
    // 2. 轮询寻找下一个任务（状态为运行且没有延时）
    do {
        task_id++;                 // 寻找下一个ID
        if(task_id >= MAX_TASKS) task_id = 0; // 如果到头了就回绕
    } while(task_state[task_id] == SUSPEND || task_timer[task_id] > 0);
    
    SP = task_sp[task_id];         // 3. 恢复下一个任务的栈指针到硬件SP
}

// --- 定时器0中断：系统的“心跳” ---
void Timer0_Routine() interrupt 1 {
    unsigned char i;
    TL0 = 0xF0;		// 设置定时初值（10ms @ 12MHz）
    TH0 = 0xD8;		
    
    for(i = 0; i < MAX_TASKS; i++) { // 遍历所有任务
        if(task_timer[i] > 0) {
            task_timer[i]--;         // 延时计数器递减
        }
    }
}

// --- 定时器初始化 (10毫秒) ---
void Timer0_Init(void) {
    TMOD &= 0xF0;	// 设置定时器模式
    TMOD |= 0x01;	// 设置定时器模式
    TL0 = 0xF0;		// 设置定时初值
    TH0 = 0xD8;		
    TF0 = 0;		// 清除TF0标志
    TR0 = 1;		// 定时器0开始计时
    ET0 = 1;        // 开启定时器中断
    EA = 1;         // 开启总中断
}