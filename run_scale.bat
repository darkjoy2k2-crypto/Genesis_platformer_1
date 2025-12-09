@echo off
cd /d "c:\Users\peter\Documents\_Genesis\Game2"
python3 scale_array.py
if errorlevel 1 (
    python scale_array.py
)
