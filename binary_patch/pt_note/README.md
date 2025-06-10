PT_NOTE 세그먼트를 이용한 바이너리 패치

원리: 프로그램 실행에 영향을 끼치지 않는 PT_NOTE를 PT_LOAD 영역으로 수정 후 패치

절차 
1. 원본 프로그램 entry_point 확인
readelf -l hello_world
2. PT_NOTE 영역에 들어갈 코드 작성. 마지막에는 원본 프로그램 entry_point로 점프하도록 설정
jmp [rel 원본 entry_point]
3. 원본 프로그램 뒤에 새로 작성한 코드 추가. (0x1000 정렬 주의)
정렬 위한 더미 파일 생성
dd count=$((정렬을 위해 추가할 파일 크기)) if=/dev/zero of=dummy bs=1
dd skip=$((새로 작성 한코드 pt_load offset)) count=$((새로 작성 한코드 pt_load 크기)) if=new if=new_load bs=1
cat 원본 프로그램 dummy new_load > patch_program

4. pt_note의 헤더 pt_load 로 수정. entry_point 수정하는 프로그램 작성