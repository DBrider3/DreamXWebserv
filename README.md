# DreamXWebserv
구닥다리 nginx를 재구성한 차세대 Webserv를 구현하는 과제입니다.

### 필요한 핵심 개념
- I/O Multiplexing이란?
    - 여러 소켓에 대해 I/O를 병행하는 기법. 
    다수의 프로세스 혹은 스레드를 만들지 않고도 여러 파일을 처리할 수 있기 때문에 less memory, less context switching이 가능해 성능 개선됨.
- kqueue란?
    - kqueue는 커널에 할당된 폴링 공간(kernel event queue - kqueue)에 모니터링할 이벤트를 등록하고, 발생한 이벤트를 return받아 Multiple I/O Event를 처리할 수 있도록 도와준다. 이벤트 등록 및 반환은 kevent 구조체를 통해 이루어지며, 구조체 필드로 존재하는 이벤트에 대한 필터, 플래그 등을 이용해 다양한 이벤트 발생 상황에 대한 정의 및 발생 이벤트에 대한 정보 확인을 할 수 있다.
        
        <aside>
        💡 폴링이란?
        "폴링"은 한 프로그램이나 장치에서 다른 프로그램이나 장치들이 어떤 상태에 있는지를 지속적으로 체크하는 전송제어 방식으로서, 대체로 그들이 아직도 접속되어 있는 지와 데이터 전송을 원하는지 등을 확인한다.
        
        </aside>
        
- kevent 구조체를 이용하여 read/write를 처리한다.
    
    ```cpp
    struct kevent {
        uintptr_t ident;        /* identifier for this event */
        int16_t   filter;       /* filter for event */
        uint16_t  flags;        /* action flags for kqueue */
        uint32_t  fflags;       /* filter flag value */
        intptr_t  data;         /* filter data value */
        void      *udata;       /* opaque user data identifier */
    };
    ```
    
    `ident:` 서버와 클라이언트를 식별한다.
    `filter(EVFILT_READ, EVFILT_WRITE etc..):` event를 핸들링한다.
    `flags:` event를 관리한다.

### 구현한 상태코드
- 구현된 상태코드표
    
    
    | 응답대역 | 응답코드 | 설명 |
    | --- | --- | --- |
    | 성공 | 200 | OK(요청이 성공적으로 수행되었음) |
    |  | 201 | Created(PUT 메소드에 의해 원격지 서버에 파일 생성됨) |
    |  | 204 | No content, (PUT, POST, DELETE 요청의 경우 성공은 했지만 정송할 데이터가 없는 경우) |
    | 리다이렉션 | 301 | Moved permanently(요구한 데이터를 변경된 타 URL에 요청함 / Redirect된 경우) |
    | 클라이언트 요청에러 | 400 | Bad Request (사용자의 잘못된 요청을 처리할 수 없음) |
    |  | 403  | Forbidden(접근 금지, 디렉터리 리스팅 요청 및 관리자 페이지 접근 등을 차단) |
    |  | 404 | Not found(요청한 페이지 없음) |
    |  | 405 | Method not allowed(허용되지 않는 http method 사용함) |
    |  | 413 | Payload Too Large (페이로드가 너무큼) |
    |  | 414 | Request-URI too long(요청 URL길이가 긴 경우) |
    | 서버에러 | 500 | Internal server error(내부 서버 오류) |

### FlowChart
![DreamX_FlowChart_v1 0](https://user-images.githubusercontent.com/38426670/190391061-ccb603cf-43f3-411c-aacd-be715e137b17.jpg)

### Cooperation Guide
[Notion Link](https://spectacular-kick-6ba.notion.site/Cooperation-Guide-3e65fe02ea204db6a0f43c284bfc68c7?pvs=4)

### 회의 자료
[Notion Link](https://spectacular-kick-6ba.notion.site/bd1d58b805624b68bcb814f84e37c91b?pvs=4)


### 속도 개선
🗣 동작 시간을 출력해보니 read에서 rfind()함수에서 소요 시간이 비약적으로 상승했다.
rfind()를 대체 할 수단을 찾아보자

💡 rfind() 들어가기 전에 빠른 수행가능한 조건을 걸어줌으로써 불필요한 rfind() 호출을 줄였다!
적용 효과 : 40-50분 가량 소요시간을 15분 정도로 줄임
