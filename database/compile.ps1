Remove-Item "../out/build/x64-debug/engine/engine.db" -Force -ErrorAction SilentlyContinue
Remove-Item "../out/build/x64-release/engine/engine.db" -Force -ErrorAction SilentlyContinue

sqlite3.exe "../out/build/x64-debug/engine/engine.db" ".read 'database.sql'"
sqlite3.exe "../out/build/x64-release/engine/engine.db" ".read 'database.sql'"