protobuf:
	@protoc --cpp_out=./ProcessMonitoring ProcessMetricas.proto

compile:
	@g++ ./ConfigAgent/ConfigAgent.cpp  \
	 main.cpp \
	./Colletion/CollectionPidNameUser.cpp \
	./ProcessMonitoring/ProcessMetricas.pb.cc \
	./Colletion/CollectionNumThreadsNice.cpp \
	./Scripts/Script.cpp \
	./API/ChannelComunication.cpp \
	./Colletion/CollectionMemory.cpp \
	./Colletion/CollectionMetricsMemoryWriteReadBytes.cpp \
	./Colletion/CollectionHostIPCreateTimeBootime.cpp \
	./AgentMonitoring/AgentMonitoring.cpp \
	-O3 -lprotobuf -std=c++17 -o monitor_process

script:
	@g++ ./AgentMonitoring/ScriptKernel.cpp ./Scripts/Script.cpp \
	./API/ChannelComunication.cpp \
	./ProcessMonitoring/ProcessMetricas.pb.cc \
	-O3 -lprotobuf -std=c++17 -o script_kernel
clean:
	rm -f *.o ProcessMetricas.pb.cc ProcessMetricas.pb.h
