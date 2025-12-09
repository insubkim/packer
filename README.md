### elf packer 
1. pt_note 세그먼트 헤더를 pt_load로 변환
2. stub 프로그램으로 entrypoint 변경
3. packed binary 생성
 
### build
**make all**

### run packer 
**./woody_woodpacker [target binary]**

<img width="892" height="358" alt="스크린샷 2025-12-09 오후 11 06 47" src="https://github.com/user-attachments/assets/1c2c6c3e-6259-49b9-ad70-937f709dd23d" />

### run packed exe
**./woody**

<img width="554" height="233" alt="스크린샷 2025-12-09 오후 11 07 00" src="https://github.com/user-attachments/assets/b426019c-7748-42e5-bc89-9808c72670aa" />
