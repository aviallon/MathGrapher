##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=MathGrapher
ConfigurationName      :=Debug
WorkspacePath          :=/home/aviallon/git/MathGrapher
ProjectPath            :=/home/aviallon/git/MathGrapher
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Antoine Viallon
Date                   :=10/17/19
CodeLitePath           :=/home/aviallon/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="MathGrapher.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            := -fopenmp -Wl,--format=binary -Wl,allegro/fonts/Arimo-Regular.ttf -Wl,--format=default 
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)allegro $(LibrarySwitch)allegro_image $(LibrarySwitch)allegro_primitives $(LibrarySwitch)allegro_memfile $(LibrarySwitch)allegro_ttf $(LibrarySwitch)allegro_font $(LibrarySwitch)allegro_dialog 
ArLibs                 :=  "allegro" "allegro_image" "allegro_primitives" "allegro_memfile" "allegro_ttf" "allegro_font" "allegro_dialog" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS := -fopenmp -std=c++14 -g -Og -Wall $(Preprocessors)
CFLAGS   :=  -g -Og -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/allegro_allegro.cpp$(ObjectSuffix) $(IntermediateDirectory)/allegro_mouse.cpp$(ObjectSuffix) $(IntermediateDirectory)/allegro_gui.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/aviallon/git/MathGrapher/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix) main.cpp

$(IntermediateDirectory)/allegro_allegro.cpp$(ObjectSuffix): allegro/allegro.cpp $(IntermediateDirectory)/allegro_allegro.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/aviallon/git/MathGrapher/allegro/allegro.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/allegro_allegro.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/allegro_allegro.cpp$(DependSuffix): allegro/allegro.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/allegro_allegro.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/allegro_allegro.cpp$(DependSuffix) -MM allegro/allegro.cpp

$(IntermediateDirectory)/allegro_allegro.cpp$(PreprocessSuffix): allegro/allegro.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/allegro_allegro.cpp$(PreprocessSuffix) allegro/allegro.cpp

$(IntermediateDirectory)/allegro_mouse.cpp$(ObjectSuffix): allegro/mouse.cpp $(IntermediateDirectory)/allegro_mouse.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/aviallon/git/MathGrapher/allegro/mouse.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/allegro_mouse.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/allegro_mouse.cpp$(DependSuffix): allegro/mouse.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/allegro_mouse.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/allegro_mouse.cpp$(DependSuffix) -MM allegro/mouse.cpp

$(IntermediateDirectory)/allegro_mouse.cpp$(PreprocessSuffix): allegro/mouse.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/allegro_mouse.cpp$(PreprocessSuffix) allegro/mouse.cpp

$(IntermediateDirectory)/allegro_gui.cpp$(ObjectSuffix): allegro/gui.cpp $(IntermediateDirectory)/allegro_gui.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/aviallon/git/MathGrapher/allegro/gui.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/allegro_gui.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/allegro_gui.cpp$(DependSuffix): allegro/gui.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/allegro_gui.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/allegro_gui.cpp$(DependSuffix) -MM allegro/gui.cpp

$(IntermediateDirectory)/allegro_gui.cpp$(PreprocessSuffix): allegro/gui.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/allegro_gui.cpp$(PreprocessSuffix) allegro/gui.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


