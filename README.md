# Operating Systems  
### Ex7
### written by: zeev fischer & almog david
### ids: 318960242 , 207749441
### project link https://github.com/zeevfischer/OPS_EX5.git  
הערה לבודקת:  
בהתחלה נאמר לנו שיש כמה חתימות של פונקציות שאפשר לשנות/ לא לממש   
אחרי זה נשלח מייל שאמר שכול החתימות צריכות להיות כמו שכתוב במטלה ואי אפשר לשנות   
רוב החתימות אצלי אכן תואמות יש כמה חתימות שלא הצלחתי לשנות ולכן הם לא תואמות מקווה להתחשבות בנושא   
החתימות הם:  
int myopendir(const char *name);  
struct mydirent *myreaddir(int fd);   
int myclosedir(int fd);   

---

### How to run
* in the terminal run make all  
* then run the server by running (./server)  
* open a second terminal and run the client by running (./client 127.0.0.1)  
* then you can start palying wit the stack with the following commands "PUSH" "POP" "TOP"  
* to stop and exit enter "EXIT"  

### Test
* to run the test follow steps 1,2 above  
* then in a second terminal run (./main 127.0.0.1)  
* this will run the mainTest file witch will run the ClientTest file

