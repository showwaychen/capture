#pragma once

// Use Window or Posix 
#include <windows.h> 


  
class base_Mutex{ 
private: 

     CRITICAL_SECTION _mutex; /**< Window mutex */
     bool _locked;           /**< Fast locked look up used for copying */
  
     void init(){ 
          InitializeCriticalSection(&_mutex); 
          _locked = false; 
     } 
  
public: 
  
     /** 
     * @brief Construct a BMutex 
     * @brief Posix and Win mutex 
     */
     base_Mutex(){ 
          init(); 
     } 
     /** 
     * @brief Copy Constructor a mutex (copy the locked state only) 
     * @param Based mutex 
     * 
     */
     base_Mutex( const base_Mutex &in_mutex ) { 
          init(); 
  
          if(in_mutex._locked && !_locked) lock(); 
          else if(!in_mutex._locked && _locked) unlock(); 
     } 
  
     /** 
     * @brief Copy a mutex (copy the locked state only) 
     * @param Based mutex 
     * @return Current mutex 
     */
     base_Mutex& operator=(const base_Mutex &in_mutex) { 
          if(in_mutex._locked && !_locked) lock(); 
          else if(!in_mutex._locked && _locked) unlock(); 
          return *this; 
     } 
  
     /** 
     * @brief Destructor 
     */
     virtual ~base_Mutex(){ 
          DeleteCriticalSection(&_mutex); 
    
     } 
  
     /** 
     * @brief lock a mutex 
     * @return WIN true 
     */
     bool lock(){ 
          _locked = true; 
          EnterCriticalSection(&_mutex); 
          return true; 
     } 
  
     /** 
     * @brief lock a mutex 
     * @return true if success else false (if busy or error) 
     */
     bool tryLock(){ 
          _locked = true; 
           if(TryEnterCriticalSection(&_mutex)) 
		      return true;
		   else
		      return false;
     } 
  
     /** 
     * @brief unlock a mutex 
     * @return WIN true in every cases 
     */
     bool unlock(){ 
          _locked = false; 
          LeaveCriticalSection(&_mutex); 
          return true; 
    
     } 
  
     /** 
     * @brief Fast locked look up 
     * @return true if locked else false 
     * This methode use the fast look up variable but still CONST 
     * if you want to test perfectly do : 
     * if(myMutex.tryLock()){ 
     *      myMutex.unlock(); 
     *      // I am sure that the mutex is not locked 
     * } 
     * else{ 
     *      // The mutex is locked 
     * } 
     */
     bool isLocked() const{ 
          return _locked; 
     } 
}; 

class base_Lock
{
public:
    // Acquire the state of the mutex
    base_Lock ( base_Mutex & mutex )
        : _mutex(mutex)
    {
        _mutex.lock();
    }
    // Release the state of the mutex
    ~base_Lock ()
    {
        _mutex.unlock();
    }
private:
    base_Mutex & _mutex;
};
