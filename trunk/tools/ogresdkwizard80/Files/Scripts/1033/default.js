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
	var bSamplesFolder = wizard.FindSymbol('LOCATION_SAMPLES');
	var bOgrenewLevelFolder = wizard.FindSymbol('LOCATION_OGRENEW');
	var bOtherFolder = wizard.FindSymbol('LOCATION_OTHER');
	var bGlobalEnvar = wizard.FindSymbol('LOCATION_ENVVAR');
	var bCommonFramework = wizard.FindSymbol('FRAMEWORK_YES');
	var bPostbuildCopy = wizard.FindSymbol('POSTBUILD_COPY');
	var sOgreFolder = wizard.FindSymbol('OGRE_FOLDER');
	var bUseCEGUI = wizard.FindSymbol('CEGUI_YES');
	
	var strCommonDir = '';
	if(bSamplesFolder)
		strCommonDir = '..\\..\\Common';
	if(bOgrenewLevelFolder)
		strCommonDir = '..\\..\\ogrenew\\samples\\common';
	if(bOtherFolder)
		strCommonDir = sOgreFolder + '\\samples\\common';
	if(bGlobalEnvar)
		strCommonDir = '$(OGRE_SRC)' + '\\samples\\common';

	var strOgreMainDir = '';
	if(bSamplesFolder)
		strOgreMainDir = '..\\..\\..\\OgreMain';
	if(bOgrenewLevelFolder)
		strOgreMainDir = '..\\..\\ogrenew\\OgreMain';
	if(bOtherFolder)
		strOgreMainDir = sOgreFolder + '\\OgreMain';
	if(bGlobalEnvar)
		strOgreMainDir = '$(OGRE_SRC)' + '\\OgreMain';

	var strDependenciesDir = '';
	if(bSamplesFolder)
		strDependenciesDir = '..\\..\\..\\Dependencies';
	if(bOgrenewLevelFolder)
		strDependenciesDir = '..\\..\\ogrenew\\Dependencies';
	if(bOtherFolder)
		strDependenciesDir = sOgreFolder + '\\Dependencies';
	if(bGlobalEnvar)
		strDependenciesDir = '$(OGRE_SRC)' + '\\Dependencies';

	var strCEGUIRendererDir = '';
	if(bSamplesFolder)
		strCEGUIRendererDir = '..\\..\\Common\\CEGUIRenderer';
	if(bOgrenewLevelFolder)
		strCEGUIRendererDir = '..\\..\\ogrenew\\samples\\Common\\CEGUIRenderer';
	if(bOtherFolder)
		strCEGUIRendererDir = sOgreFolder + '\\samples\\Common\\CEGUIRenderer';
	if(bGlobalEnvar)
		strCEGUIRendererDir = '$(OGRE_SRC)' + '\\samples\\Common\\CEGUIRenderer';
	
	
//DEBUG//////////////////////////////////////////////////////
//set configurations for debug|win32

	var config = proj.Object.Configurations("Debug|Win32");
	config.IntermediateDirectory = 'obj\\$(ConfigurationName)';
	config.OutputDirectory = 'bin\\$(ConfigurationName)';
	config.CharacterSet = charSetMBCS;
	
	var debugSettings = config.DebugSettings;
	if(bPostbuildCopy)
	{
		debugSettings.WorkingDirectory = strCommonDir + "\\Bin\\$(ConfigurationName)";
		debugSettings.Command = strCommonDir + "\\Bin\\$(ConfigurationName)\\$(ProjectName).exe";
	}
	else
	{
		debugSettings.WorkingDirectory = "Bin\\$(ConfigurationName)";
		debugSettings.Command = "Bin\\$(ConfigurationName)\\$(ProjectName).exe";
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

	var strAdditionalIncludeDirectories = '';
	if(bCommonFramework)
	{
		strAdditionalIncludeDirectories = "include;" + strCommonDir + "\\include;" + strOgreMainDir + "\\include";
	}
	else
	{
		strAdditionalIncludeDirectories = "include;" + strOgreMainDir + "\\include";
	}

	if(bUseCEGUI)
		strAdditionalIncludeDirectories = strAdditionalIncludeDirectories + ";" + strDependenciesDir + "\\include\\CEGUI" + ";" + strCEGUIRendererDir + "\\include;" + strDependenciesDir + "\\include";

	CLTool.AdditionalIncludeDirectories = strAdditionalIncludeDirectories;


	var LinkTool = config.Tools("VCLinkerTool");

	LinkTool.ProgramDatabaseFile = "$(TargetDir)\\$(TargetName)" + ".pdb";
	LinkTool.GenerateDebugInformation = true;
	LinkTool.ProgramDatabaseFile = "$(OutDir)/" + strProjectName + ".pdb";
	LinkTool.LinkIncremental = linkIncrementalYes;
	LinkTool.SubSystem = subSystemWindows;
	LinkTool.OutputFile = "$(OutDir)\\$(ProjectName)" + ".exe";
	LinkTool.TargetMachine = machineTypeOption.machineX86;

	var strAdditionalLibraryDirectories = '';
	var strAdditionalDependencies = '';

	strAdditionalLibraryDirectories = strOgreMainDir + "\\lib\\$(ConfigurationName)";
	strAdditionalDependencies = "OgreMain_d.lib";

	if(bUseCEGUI)
	{
		strAdditionalLibraryDirectories = strAdditionalLibraryDirectories + ";" + strDependenciesDir + "\\lib\\$(ConfigurationName)";
		strAdditionalLibraryDirectories = strAdditionalLibraryDirectories + ";" + strCEGUIRendererDir + "\\lib";
		strAdditionalDependencies = strAdditionalDependencies + " CEGUIBase_d.lib OgreGUIRenderer_d.lib";
	}

	LinkTool.AdditionalLibraryDirectories = strAdditionalLibraryDirectories;
	LinkTool.AdditionalDependencies = strAdditionalDependencies;

	if(bPostbuildCopy)
	{
		var PostBuildTool = config.Tools("VCPostBuildEventTool");
		PostBuildTool.Description = "Copying exe to samples bin directory ...";

		PostBuildTool.CommandLine = "copy $(OutDir)\\$(TargetFileName) " + strCommonDir + "\\Bin\\$(ConfigurationName)";
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
	
	var strAdditionalIncludeDirectories = '';
	if(bCommonFramework)
	{
		strAdditionalIncludeDirectories = "include;" + strCommonDir + "\\include;" + strOgreMainDir +"\\include";
	}
	else
	{
		strAdditionalIncludeDirectories = "include;" + strOgreMainDir + "\\include";
	}
	
	if(bUseCEGUI)
		strAdditionalIncludeDirectories = strAdditionalIncludeDirectories + ";" + strDependenciesDir + "\\include\\CEGUI" + ";" + strCEGUIRendererDir + "\\include;" + strDependenciesDir + "\\include";
	
	CLTool.AdditionalIncludeDirectories = strAdditionalIncludeDirectories;



	var LinkTool = config.Tools("VCLinkerTool");
	LinkTool.GenerateDebugInformation = true;
	LinkTool.LinkIncremental = linkIncrementalNo;

	LinkTool.SubSystem = subSystemWindows;

	LinkTool.OutputFile = "$(OutDir)\\$(ProjectName)" + ".exe";

	var strAdditionalLibraryDirectories = '';
	strAdditionalLibraryDirectories = strOgreMainDir + "\\lib\\$(ConfigurationName)";
	
	var strAdditionalDependencies = '';
	strAdditionalDependencies = "OgreMain.lib";
	
	if(bUseCEGUI)
	{
		strAdditionalLibraryDirectories = strAdditionalLibraryDirectories + ";" + strDependenciesDir + "\\lib\\$(ConfigurationName)";
		strAdditionalLibraryDirectories = strAdditionalLibraryDirectories + ";" + strCEGUIRendererDir + "\\lib";
		strAdditionalDependencies = strAdditionalDependencies + " CEGUIBase.lib OgreGUIRenderer.lib";
	}
	
	LinkTool.AdditionalLibraryDirectories = strAdditionalLibraryDirectories;
	LinkTool.AdditionalDependencies = strAdditionalDependencies;
	
	if(bPostbuildCopy)
	{
		var PostBuildTool = config.Tools("VCPostBuildEventTool");
		PostBuildTool.Description = "Copying exe to samples bin directory ...";

		PostBuildTool.CommandLine = "copy $(OutDir)\\$(TargetFileName) " + strCommonDir + "\\Bin\\$(ConfigurationName)";
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
