## 실행 시 주의사항
-FBXSD/lib/release/libfbxsdk-md

-FBXSD/lib/release/libfbxsdk-mt
파일 추가시 빌드가능


## DirectX11 3D MapTool
  
DirectX11 을 이용한 맵툴제작
* 제작기간 : 2023.12 ~ 2024.02
 <br>
* 시연 영상 : https://www.youtube.com/watch?v=LpMHkSxpFdY
 <br>
* DownloadLink : https://drive.google.com/file/d/1-WgzNXx6OKWGYwIPq4wMHWlw6BUvFI9R/view?usp=drive_link
## 조작법

* W,A,S,D - 이동
* Q - 상승
* E  - 하강
* 마우스 좌드래그 - 화면 이동
* 마우스 우클릭  - 오브젝트 배치 및 스플래팅  

## 지형조작

![Alt text](ReadmeImage/1.gif)
* 마우스 피킹을 이용하여 선택된 지점의 지형을 올리기 및 내리기가 가능합니다.
  
## Undo/Redo  
![Alt text](ReadmeImage/2.gif)
* 작업한 내용을 vector로 저장하여 Undo 및 Redo기능 사용 가능합니다.
## Splatting
![Alt text](ReadmeImage/3.gif)
* 멀티 텍스쳐링을 이용하여 선택된 지점의 픽셀 컬러를 원하는 텍스쳐의 컬러로 드로잉 가능합니다.

## 오브젝트 배치
![Alt text](ReadmeImage/4.gif)
* 원하는 오브젝트를 마우스 피킹 지점에 배치 가능합니다.

## 오브젝트 선택,삭제,SRT조정
![Alt text](ReadmeImage/5.gif)
* 오브젝트를 선택하여 삭제 및 SRT를 조정 가능합니다.
  
## NewMap 생성
![Alt text](ReadmeImage/6.gif)
* 원하는 사이즈를 입력하여 새로운 맵을 생성 할 수 있습니다.

## Fog
![Alt text](ReadmeImage/7.gif)
* 카메라와의 거리를 계산하여 거리가 멀어질 수록
  Fog값을 계산하여 PixelShader에서 더해주어 
  Fog효과를 표현했습니다.

## Load HeightMap
![Alt text](ReadmeImage/8.gif)
* 원하는 높이맵을 불러와 맵에 적용 가능합니다.

## LoadMap
![Alt text](ReadmeImage/9.gif)
* 이전에 저장해 놓았던 맵 정보를 불러옵니다.

## SaveMap
![Alt text](ReadmeImage/10.gif)
* 현재 맵의 정보를 저장합니다.

## Watermap/Environment Mapping
![Alt text](ReadmeImage/11.gif)
* WaterMap에 환경맵핑을 적용하여 오브젝트 및 지형을 표현가능 합니다.
## Instancing
![Alt text](ReadmeImage/12.gif)
* 오브젝트의 Instancing을 적용하여 많은 수의 오브젝트를 배치 하여도 안정적인 FPS를 유지 가능합니다.
