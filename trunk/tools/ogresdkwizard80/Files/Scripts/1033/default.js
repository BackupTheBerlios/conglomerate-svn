/* Copyright (c) Microsoft Corporation.  All rights reserved.  */

var bCopyOnly = false;

function OnFinish(selProj, selObj)
{
	try
	{
		var strProjectPath = wizard.FindSymbol("PROJECT_PATH");
		var strProjectName = wizard.FindSymbol("PROJECT_NAME");

		var bEmptyProject = wizard.FindSymbol("EMPTY_PROJECT");

		//strProjectPath += '\\scripts';

		selProj = CreateProject(strProjectName, strProjectPath);
   
		AddCommonConfig(selProj, strProjectName);
		AddSpecificConfig(selProj, strProjectName, bEmptyProject);
		
        SetupFilters(selProj);

		if (!bEmptyProject)
		{
			var InfFile = CreateInfFile();
            AddFilesToProjectWithInfFile(selProj, strProjectName);
			SetCommonPchSettings(selProj);
			InfFile.Delete();
			var projName = strProjectPath + "\\" + strProjectName + ".vcproj";
			selProj.Object.Save();
		}
	}
	catch(e)
	{
		if (e.description.length != 0)
			SetErrorInfo(e);
		return e.number
	}
}

function GetTargetName(strName, strProjectName, strResourcePath)
{
	var strTarget = strName;

		switch (strName)
		{
			case "docs/readme.txt":
				strTarget = "docs/ReadMe.txt";
				break;
			case "res/resource.h":
				strTarget = "res/Resource.h";
				break;
			case "res/root.ico":
				strTarget = "res" + "\\" + strProjectName + ".ico";
				break;
			case "res/root.rc":
				strTarget = "res" + "\\" + strProjectName + ".rc";
				break;
			case "include/root.h":
				strTarget = "include" + "\\" + strProjectName + ".h";
				break;
			case "src/root.cpp":
				strTarget = "src" + "\\" + strProjectName + ".cpp";
				break;
			default:
				break;
		}
	return strTarget; 
}


function AddSpecificConfig(proj, strProjectName, bEmptyProject)
{
	var bCommonFramework = wizard.FindSymbol('FRAMEWORK_YES');
	var bPostbuildCopy = wizard.FindSymbol('POSTBUILD_COPY');
	var sOgreFolder = wizard.FindSymbol('OGRE_FOLDER');
	var bUseCEGUI = wizard.FindSymbol('CEGUI_YES');
	var bExampleFramework = wizard.FindSymbol('FRAMEWORK_YES');
	
	strOgreMainDir = '$(OGRE_HOME)';
	strCEGUIRendererDir = '$(OGRE_HOME)' + '\\samples\\include';
	
	
//DEBUG//////////////////////////////////////////////////////
//set configurations for debug|win32

	var config = proj.Object.Configurations("Debug|Win32");
	config.IntermediateDirectory = 'obj\\$(ConfigurationName)';
	config.OutputDirectory = 'bin\\$(ConfigurationName)';
	config.CharacterSet = charSetMBCS;
	
	var debugSettings = config.DebugSettings;
	if(bPostbuildCopy)
	{
		debugSettings.WorkingDirectory = strOgreMainDir + "\\Bin\\$(ConfigurationName)";
		debugSettings.Command = strOgreMainDir + "\\Bin\\$(ConfigurationName)\\$(ProjectName).exe";
	}
	else
	{
		debugSettings.WorkingDirectory = "..\\Bin\\$(ConfigurationName)";
		debugSettings.Command = "..\\Bin\\$(ConfigurationName)\\$(ProjectName).exe";
	}


	var CLTool = config.Tools("VCCLCompilerTool");
	CLTool.UsePrecompiledHeader = pchNone;

	CLTool.RuntimeLibrary = rtMultiThreadedDebugDLL;
	CLTool.MinimalRebuild = true;
	CLTool.DebugInformationFormat = debugEditAndContinue;
	CLTool.BasicRuntimeChecks = runtimeBasicCheckAll;
	CLTool.Optimization = optimizeDisabled;
	CLTool.ExceptionHandling = cppExceptionHandlingYes;

	CLTool.Detect64BitPortabilityProblems = true;
	CLTool.EnableFunctionLevelLinking = true;
	CLTool.MinimalRebuild = true;
	CLTool.WarningLevel = WarningLevel_3;
	CLTool.PreprocessorDefinitions = "WIN32;_DEBUG;_WINDOWS";

	strAdditionalIncludeDirectories = "include;" + strOgreMainDir + "\\include";

	if(bUseCEGUI)
		strAdditionalIncludeDirectories = strAdditionalIncludeDirectories + ";" + strOgreMainDir + "\\include\\CEGUI" + ";" + strCEGUIRendererDir;

	if(bExampleFramework)
		strAdditionalIncludeDirectories = strAdditionalIncludeDirectories + ";" + strOgreMainDir + "\\samples\\include";
	
	CLTool.AdditionalIncludeDirectories = strAdditionalIncludeDirectories;


	var LinkTool = config.Tools("VCLinkerTool");

	LinkTool.ProgramDatabaseFile = "$(TargetDir)\\$(TargetName)" + ".pdb";
	LinkTool.GenerateDebugInformation = true;
	LinkTool.ProgramDatabaseFile = "$(OutDir)/" + strProjectName + ".pdb";
	LinkTool.LinkIncremental = linkIncrementalYes;
	LinkTool.SubSystem = subSystemWindows;
	LinkTool.OutputFile = "$(OutDir)\\$(ProjectName)" + ".exe";
	LinkTool.TargetMachine = machineTypeOption.machineX86;

	strAdditionalLibraryDirectories = strOgreMainDir + "\\lib";
	strAdditionalDependencies = "OgreMain_d.lib";
		
	if(bUseCEGUI)
	{
		strAdditionalDependencies = strAdditionalDependencies + " CEGUIBase_d.lib OgreGUIRenderer_d.lib";
	}

	LinkTool.AdditionalLibraryDirectories = strAdditionalLibraryDirectories;
	LinkTool.AdditionalDependencies = strAdditionalDependencies;

	if(bPostbuildCopy)
	{
		var PostBuildTool = config.Tools("VCPostBuildEventTool");
		PostBuildTool.Description = "Copying exe to samples bin directory ...";

		PostBuildTool.CommandLine = 'copy "$(OutDir)\\$(TargetFileName)" "' + strOgreMainDir + '\\Bin\\$(ConfigurationName)"';
	}
    
    
//RELEASE//////////////////////////////////////////////////////
//set configurations for release|win32

	config = proj.Object.Configurations.Item("Release|Win32");
	config.IntermediateDirectory = 'obj\\$(ConfigurationName)';
	config.OutputDirectory = 'bin\\$(ConfigurationName)';
	config.CharacterSet = charSetMBCS;
	
	var CLTool = config.Tools("VCCLCompilerTool");
	CLTool.UsePrecompiledHeader = pchNone;

	CLTool.RuntimeLibrary = rtMultiThreadedDLL;
	CLTool.ExceptionHandling = cppExceptionHandlingYes;
	CLTool.DebugInformationFormat = debugEnabled;
	CLTool.WarningLevel = WarningLevel_3;
	//CLTool.ImproveFloatingPointConsistency = true;		
	CLTool.Detect64BitPortabilityProblems = true;
	CLTool.PreprocessorDefinitions = "WIN32;NDEBUG;_WINDOWS";
	CLTool.EnableFunctionLevelLinking = true;
	CLTool.MinimalRebuild = true;
	//CLTool.FavorSizeOrSpeed = favorSizeOrSpeedOption.favorSpeed;
	//CLTool.GlobalOptimizations = true;
	//CLTool.InlineFunctionExpansion = inlineExpansionOption.expandOnlyInline;
	//CLTool.Optimization = optimizeOption.optimizeMaxSpeed;
	//CLTool.OmitFramePointers = true;
	//CLTool.OptimizeForProcessor = ProcessorOptimizeOption.procOptimizePentiumProAndAbove;
	//CLTool.OptimizeForWindowsApplication = true;
	//CLTool.StringPooling = true;
	CLTool.InlineFunctionExpansion = expandOnlyInline;
	
	strAdditionalIncludeDirectories = "include;" + strOgreMainDir + "\\include";
		
	if(bUseCEGUI)
		strAdditionalIncludeDirectories = strAdditionalIncludeDirectories + ";" + strOgreMainDir + "\\include\\CEGUI" + ";" + strCEGUIRendererDir;
	
	if(bExampleFramework)
		strAdditionalIncludeDirectories = strAdditionalIncludeDirectories + ";" + strOgreMainDir + "\\samples\\include";

	CLTool.AdditionalIncludeDirectories = strAdditionalIncludeDirectories;



	var LinkTool = config.Tools("VCLinkerTool");
	LinkTool.GenerateDebugInformation = true;
	LinkTool.LinkIncremental = linkIncrementalNo;

	LinkTool.SubSystem = subSystemWindows;

	LinkTool.OutputFile = "$(OutDir)\\$(ProjectName)" + ".exe";

	strAdditionalLibraryDirectories = strOgreMainDir + "\\lib";
		
	strAdditionalDependencies = "OgreMain.lib";
		
	if(bUseCEGUI)
	{
		strAdditionalDependencies = strAdditionalDependencies + " CEGUIBase.lib OgreGUIRenderer.lib";
	}
	
	LinkTool.AdditionalLibraryDirectories = strAdditionalLibraryDirectories;
	LinkTool.AdditionalDependencies = strAdditionalDependencies;
	
	if(bPostbuildCopy)
	{
		var PostBuildTool = config.Tools("VCPostBuildEventTool");
		PostBuildTool.Description = "Copying exe to samples bin directory ...";

		PostBuildTool.CommandLine = 'copy "$(OutDir)\\$(TargetFileName)" "' + strOgreMainDir + '\\Bin\\$(ConfigurationName)"';
	}

}

function SetFileProperties(projfile, strName)
{
	return false;
}

function DoOpenFile(strTarget)
{
	return false;
}
