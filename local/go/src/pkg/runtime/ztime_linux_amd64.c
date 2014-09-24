// auto generated by go tool dist
// goos=linux goarch=amd64

#include "runtime.h"
#include "defs_GOOS_GOARCH.h"
#include "os_GOOS.h"
#include "arch_GOARCH.h"
#include "malloc.h"
#include "race.h"

#line 16 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
static Timers timers; 
static void addtimer ( Timer* ) ; 
void
time·Sleep(int64 ns)
{
#line 25 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"

	runtime·tsleep(ns, "sleep");
}
void
time·startTimer(Timer* t)
{
#line 30 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"

	if(raceenabled)
		runtime·racerelease(t);
	runtime·addtimer(t);
}
void
time·stopTimer(Timer* t, bool stopped)
{
#line 38 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"

	stopped = runtime·deltimer(t);
	FLUSH(&stopped);
}

#line 44 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
static void timerproc ( void ) ; 
static void siftup ( int32 ) ; 
static void siftdown ( int32 ) ; 
#line 49 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
static void 
ready ( int64 now , Eface e ) 
{ 
USED ( now ) ; 
#line 54 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
runtime·ready ( e.data ) ; 
} 
#line 57 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
static FuncVal readyv = { ( void ( * ) ( void ) ) ready } ; 
#line 60 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
void 
runtime·tsleep ( int64 ns , int8 *reason ) 
{ 
Timer t; 
#line 65 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
if ( ns <= 0 ) 
return; 
#line 68 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
t.when = runtime·nanotime ( ) + ns; 
t.period = 0; 
t.fv = &readyv; 
t.arg.data = g; 
runtime·lock ( &timers ) ; 
addtimer ( &t ) ; 
runtime·park ( runtime·unlock , &timers , reason ) ; 
} 
#line 77 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
static FuncVal timerprocv = { timerproc } ; 
#line 79 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
void 
runtime·addtimer ( Timer *t ) 
{ 
runtime·lock ( &timers ) ; 
addtimer ( t ) ; 
runtime·unlock ( &timers ) ; 
} 
#line 89 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
static void 
addtimer ( Timer *t ) 
{ 
int32 n; 
Timer **nt; 
#line 95 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
if ( timers.len >= timers.cap ) { 
#line 97 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
n = 16; 
if ( n <= timers.cap ) 
n = timers.cap*3 / 2; 
nt = runtime·malloc ( n*sizeof nt[0] ) ; 
runtime·memmove ( nt , timers.t , timers.len*sizeof nt[0] ) ; 
runtime·free ( timers.t ) ; 
timers.t = nt; 
timers.cap = n; 
} 
t->i = timers.len++; 
timers.t[t->i] = t; 
siftup ( t->i ) ; 
if ( t->i == 0 ) { 
#line 111 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
if ( timers.sleeping ) { 
timers.sleeping = false; 
runtime·notewakeup ( &timers.waitnote ) ; 
} 
if ( timers.rescheduling ) { 
timers.rescheduling = false; 
runtime·ready ( timers.timerproc ) ; 
} 
} 
if ( timers.timerproc == nil ) { 
timers.timerproc = runtime·newproc1 ( &timerprocv , nil , 0 , 0 , addtimer ) ; 
timers.timerproc->issystem = true; 
} 
} 
#line 129 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
bool 
runtime·deltimer ( Timer *t ) 
{ 
int32 i; 
#line 136 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
i = t->i; 
USED ( i ) ; 
#line 139 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
runtime·lock ( &timers ) ; 
#line 144 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
i = t->i; 
if ( i < 0 || i >= timers.len || timers.t[i] != t ) { 
runtime·unlock ( &timers ) ; 
return false; 
} 
#line 150 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
timers.len--; 
if ( i == timers.len ) { 
timers.t[i] = nil; 
} else { 
timers.t[i] = timers.t[timers.len]; 
timers.t[timers.len] = nil; 
timers.t[i]->i = i; 
siftup ( i ) ; 
siftdown ( i ) ; 
} 
runtime·unlock ( &timers ) ; 
return true; 
} 
#line 168 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
static void 
timerproc ( void ) 
{ 
int64 delta , now; 
Timer *t; 
void ( *f ) ( int64 , Eface ) ; 
Eface arg; 
#line 176 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
for ( ;; ) { 
runtime·lock ( &timers ) ; 
now = runtime·nanotime ( ) ; 
for ( ;; ) { 
if ( timers.len == 0 ) { 
delta = -1; 
break; 
} 
t = timers.t[0]; 
delta = t->when - now; 
if ( delta > 0 ) 
break; 
if ( t->period > 0 ) { 
#line 190 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
t->when += t->period * ( 1 + -delta/t->period ) ; 
siftdown ( 0 ) ; 
} else { 
#line 194 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
timers.t[0] = timers.t[--timers.len]; 
timers.t[0]->i = 0; 
siftdown ( 0 ) ; 
t->i = -1; 
} 
f = ( void* ) t->fv->fn; 
arg = t->arg; 
runtime·unlock ( &timers ) ; 
if ( raceenabled ) 
runtime·raceacquire ( t ) ; 
f ( now , arg ) ; 
runtime·lock ( &timers ) ; 
} 
if ( delta < 0 ) { 
#line 209 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
timers.rescheduling = true; 
runtime·park ( runtime·unlock , &timers , "timer goroutine (idle)" ) ; 
continue; 
} 
#line 214 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
timers.sleeping = true; 
runtime·noteclear ( &timers.waitnote ) ; 
runtime·unlock ( &timers ) ; 
runtime·entersyscallblock ( ) ; 
runtime·notetsleep ( &timers.waitnote , delta ) ; 
runtime·exitsyscall ( ) ; 
} 
} 
#line 225 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
static void 
siftup ( int32 i ) 
{ 
int32 p; 
Timer **t , *tmp; 
#line 231 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
t = timers.t; 
while ( i > 0 ) { 
p = ( i-1 ) /2; 
if ( t[i]->when >= t[p]->when ) 
break; 
tmp = t[i]; 
t[i] = t[p]; 
t[p] = tmp; 
t[i]->i = i; 
t[p]->i = p; 
i = p; 
} 
} 
#line 245 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
static void 
siftdown ( int32 i ) 
{ 
int32 c , len; 
Timer **t , *tmp; 
#line 251 "/tmp/bindist907131767/go/src/pkg/runtime/time.goc"
t = timers.t; 
len = timers.len; 
for ( ;; ) { 
c = i*2 + 1; 
if ( c >= len ) { 
break; 
} 
if ( c+1 < len && t[c+1]->when < t[c]->when ) 
c++; 
if ( t[c]->when >= t[i]->when ) 
break; 
tmp = t[i]; 
t[i] = t[c]; 
t[c] = tmp; 
t[i]->i = i; 
t[c]->i = c; 
i = c; 
} 
} 