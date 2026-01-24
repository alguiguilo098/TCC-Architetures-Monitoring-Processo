protobuf:
	@protoc --cpp_out=./ProcessMonitoring ProcessMetricas.proto

compile:
	@g++ ./ConfigAgent/ConfigAgent.cpp  \
	 main.cpp \
	./Colletion/CollectionPidNameUser.cpp \
	./ProcessMonitoring/ProcessMetricas.pb.cc \
	./Colletion/CollectionNumThreadsNice.cpp \
	./Scripts/Script.cpp \
	./Colletion/CollectionHostIPCreateTimeBootime.cpp \
	./AgentMonitoring/AgentMonitoring.cpp \
	-O3 -lprotobuf -std=c++17 -o monitor_process
clean:
	rm -f *.o ProcessMetricas.pb.cc ProcessMetricas.pb.h
