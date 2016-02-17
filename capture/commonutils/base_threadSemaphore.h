#pragma once
  

#include <windows.h> 
#define LMAXIMUMCOUNT 99999999 /**< Maximum semaphore value in Windows*/ 

 
  
class CBase_threadSemaphore{ 
protected: 
    HANDLE _sem;    

public: 
    /** 
    * @brief Constructor 
    * @param in_init original value 
    */
    CBase_threadSemaphore( int in_init = 0 ){ 
        _sem = CreateSemaphore(0,in_init,LMAXIMUMCOUNT,0); 
    } 
  
    /** 
    * @brief Copy constructor 
    * @param in_sem original semaphore 
    */
    CBase_threadSemaphore(const CBase_threadSemaphore &in_sem){ 
        int value = in_sem.value(); 
        _sem = CreateSemaphore(0,value,LMAXIMUMCOUNT,0); 
 
    } 
  
    /** 
    * @brief Copy method 
    * @param in_sem original semaphore 
    */
    void operator=(const CBase_threadSemaphore &in_sem){ 
        reset(in_sem.value()); 
    } 
  
    /** 
    * @brief destroy semaphore 
    */
    virtual ~CBase_threadSemaphore(){ 
        CloseHandle(_sem); 
   
    } 
  
    /** 
    * @brief Wait until the semaphore is called by another thread 
    * @return true if success or false if timeout or error 
    */
    bool wait() const { 
        return WaitForSingleObject(static_cast< HANDLE >(_sem),INFINITE) == 0x00000000L; 

    } 
  
    /** 
    * @brief post a token 
    * @return true if success or false if error 
    */
    bool post(){ 
        return ReleaseSemaphore(static_cast< HANDLE >(_sem),1,0) != 0; 
    } 
  
    /** 
    * @brief get current value 
    * @return value 
    */
    int value() const{ 
        long value = -1; 
        ReleaseSemaphore(static_cast<const HANDLE>(_sem),0,&value); 
        return value; 
  
    } 
  
    /** 
    * @brief release current semaphore and create a new one 
    * @param init the value after reset 
    */
    void reset( int in_init = 0 ){
        CloseHandle(_sem); 
        _sem = CreateSemaphore(0,in_init,LMAXIMUMCOUNT,0); 
    } 
}; 
