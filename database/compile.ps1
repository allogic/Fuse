Remove-Item "../out/build/x64-debug/engine.db" -Force -ErrorAction SilentlyContinue
Remove-Item "../out/build/x64-release/engine.db" -Force -ErrorAction SilentlyContinue

sqlite3.exe "../out/build/x64-debug/engine.db" ".read 'database.sql'"
sqlite3.exe "../out/build/x64-release/engine.db" ".read 'database.sql'"