@echo off
echo ========================================
echo Auto Push Script - Le Trong Kha
echo Student ID: 22t1020634
echo ========================================

cd "C:\Users\le trong kha\Downloads"

echo Removing old repository...
rmdir /s /q "2025-2026.2.TIN4024.002" 2>nul

echo Cloning fresh repository...
git clone https://github.com/vvdung-husc/2025-2026.2.TIN4024.002.git

echo Copying updated files...
robocopy "2025-2026.2.TIN4024.002-main\TEAM_01\22t1020634_LeTrongKha" "2025-2026.2.TIN4024.002\TEAM_01\22t1020634_LeTrongKha" /E /Y

cd "2025-2026.2.TIN4024.002"

echo Configuring Git...
git config --global user.name "Khale2408"
git config --global user.email "22t1020634@husc.edu.vn"

echo Adding files...
git add TEAM_01/22t1020634_LeTrongKha/

echo Committing...
git commit -m "Update LED_Blink project - Le Trong Kha (22t1020634)"

echo Pushing to GitHub...
git push origin main

echo ========================================
echo Push completed successfully!
echo ========================================
pause