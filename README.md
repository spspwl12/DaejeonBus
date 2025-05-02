# DaejeonBus

## 프로젝트 소개
대전 버스 노선을 관리하기 쉽게 하는 취지로 만든 프로그램이며, C언어와 WinAPI를 사용해 구현한 프로젝트 입니다.<br>
이 프로젝트는 제가 국비학원에 다니는 동안 제작한 프로그램 입니다.<br>
이 프로그램에 사용된 이미지, 노선 데이터는 다음과 같습니다.
- 네이버 지도 ( https://map.naver.com ) => 타일 단위로 다운받아 프로그램에 사용했습니다.
- 대전 교통 정보센터 ( https://traffic.daejeon.go.kr ) => 노선과 버스 데이터, 정류장 데이터를 추출했습니다.

## 주요 특징
- 버스 추가: 버스 정보를 입력해 버스를 새로 추가 할 수 있습니다.
- 정류장 추가: 정류장 정보를 입력해 정류장을 새로 추가 할 수 있습니다.
- 버스 노선 추가/수정: 버스 노선을 추가하거나 수정이 가능합니다.
- 지도 뷰어: 네이버 지도에서 추출한 타일 이미지를 Gdi+ api를 활용해 지도를 구현했습니다.

## 컴파일 및 실행 방법
1. Visual Studio 2022 를 다운로드 합니다. ( https://visualstudio.microsoft.com/ko/thank-you-downloading-visual-studio/?sku=Community&channel=Release&version=VS2022&source=VSLandingPage&cid=2030&passive=false )
2. Visual Studio Installer 가 뜨면 
![a](https://github.com/user-attachments/assets/d213d50a-fec8-4166-87e6-651e8fe761ce)
 와 같이 체크를 합니다.
3. 설치 (Install)를 합니다.
4. 프로젝트를 다운로드 합니다.<br>
![Animation](https://github.com/user-attachments/assets/42c9f723-c589-439b-bb83-8c5b43604cf2)
5. 압축을 풉니다.
6. 맵 파일을 별도로 다운받습니다.
   ![Animation2](https://github.com/user-attachments/assets/342c118a-976d-4b86-a393-bcd32a7f000d)
7. DaejeonBus.sln 파일을 더블클릭 해 프로젝트를 엽니다.
8. 플랫폼을 x86 그리고 구성을 Release 로 설정합니다.<br>
  ![image](https://github.com/user-attachments/assets/7af27765-d8c5-4e0c-bac5-6a8f7a60473c)
9. F7를 눌러 컴파일 합니다.
10. 컴파일이 완료되면, 프로젝트 폴더안의 Build 폴더에 있는 BusClient.exe 또는 DaejeonBus.exe 를 실행합니다.
    
## 작동 화면
### BusClient
![Animation](https://github.com/user-attachments/assets/132fbf84-46ab-4fa6-8e17-c5dff2258ac1)
![Animation](https://github.com/user-attachments/assets/40a02936-963a-4d26-bf20-8fa6cfd08397)
![Animation2](https://github.com/user-attachments/assets/4d20de5d-74e3-4ae4-8d00-85ac0882cd87)

### DaejeonBus 관리

