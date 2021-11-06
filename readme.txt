1. 부팅시 Logo 출력
2. 상단에 측정 기록된 거리 출력. 초기 출력 Distance 00 Track Round 3, Time Over 30, Best Record 00:00:00 Best Average 00:00:00
3. A 버튼 클릭 안내문 Press the A button to correct the distance.
4. Calibration 종료 후, Distance 09 출력
5. 화면에 Game Start, Button A
6. 부저 사운드와 함께 3, 2, 1, Go 출력
7. 처음 차량 인지하면서 기록 시간 시작
8. 총 3회 측정애서 게임 종료 함
9. 게임중에는 버튼 입력 불가. 단, 한 트랙 측정 중 1분 초과시 자동 종료
10. 게임 종료 후 다시 A버튼 누르면 게임 시작
11. Game start 화면이나 종료된 화면에서 B 버튼 누르면 설정 화면 진입
12. 1. Calibrate 2. Track 1~5(3), Time over 20~90(30), Best track time(기본), Best Average time 선택

Font 정보
https://github.com/olikraus/u8glib/wiki/fontsize
u8g.setFont(u8g_font_helvB10);
u8g.setFont(u8g_font_helvB12);

Line 2
u8g.drawBox(0,27,128,24);

Line 3
u8g.drawBox(0,48,128,24);


1. 버튼을 누르기전
    1. 공백
    2. Push A Button
    3. To check distance
2. 버튼 1번 
    1. 거리 측정 값1, 값2, 값3
    2. Calibration
    3. In Progress
3. 캘리브레이션 완료(자동 출력)
    1. 캘리브레이션 결과
    2. Calibration
    3. Completed
4. 버튼 2번
    1. 캘리브레이션 결과
    2. Push a button
    3. To START
5. 출발 신호 소리
    1. 캘리브레이션 결과
    2. 3,2,1
    3. Ready
6. 출발 신호 마지막 소리
    1. 캘리브레이션 결과
    2. Let's Go
    3. START !!!
7. 차량 출발
    1. 캘리브레이션 결과, 차량 측정 결과, 라운드 표시 1
    2. 출발 후 흐른 시간
    3. 차량 검출 후 흐른 시간
8. 차량 검출
    1. 캘리브레이션 결과, 차량 측정 결과, 라운드 표시2
    2. 출발 후 흐른 시간
    3. 차량 검출 후 흐른 시간
9. 차량 검출
    1. 캘리브레이션 결과, 차량 측정 결과, 라운드 표시3
    2. 출발 후 흐른 시간
    3. 차량 검출 후 흐른 시간
10. 차량 검출
    1. Best Round 2 - 0:00:00
    2. 출발 후 흐른 시간 멈춤
    3. Round 1 - 0:00:00 Round 2 - 0:00:00 Round 3 - 0:00:00 흐르기...