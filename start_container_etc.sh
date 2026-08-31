#Команды_для_работы_с_проектом
docker build \
  --build-arg USER_UID=$(id -u) \
  --build-arg USER_GID=$(id -g) \
  -t mid-5-geometric \
  -f .devcontainer/Dockerfile .


#1. Зашел в Докер (VS Code написал в углу Dev Container: ...).
#2. Запустил Conan (если добавил новые библиотеки): он скачает их и создаст пресеты.
#conan install . --build=missing -s build_type=Debug

#3. Если был шаг 2 запустил CMake руками из корня через пресет: 
#cmake --preset conan-debug

#4. Скомпилировал руками: 

#весь проект
#cmake --build --preset conan-debug  

#отдельный таргет
#cmake --build build --target metric_accumulator


#5 Закрыть проект (там где VS Code написал в углу Dev Container: ...) нажать, в меню сверху выбрать.
#Close remote connection
