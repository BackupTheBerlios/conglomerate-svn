
function OnFinish(selProj, selObj)
{
	try
	{
		var strProjectPath = wizard.FindSymbol('PROJECT_PATH');
		var strProjectName = wizard.FindSymbol('PROJECT_NAME');
		var bEmptyProject = wizard.FindSymbol('EMPTY_PROJECT');

		strProjectPath += '\\scripts';

		selProj = CreateCustomProject(strProjectName, strProjectPath);
		AddConfig(selProj, strProjectName);
		AddFilters(selProj);

		if (!bEmptyProject)
		{
			var InfFile = CreateCustomInfFile();
			AddFilesToCustomProj(selProj, strProjectName, strProjectPath, InfFile);
			PchSettings(selProj);
			InfFile.Delete();
		}
		selProj.Object.Save();
	}
	catch(e)
	{
		if (e.description.length != 0)
			SetErrorInfo(e);
		return e.number
	}
}

function CreateCustomProject(strProjectName, strProjectPath)
{
	try
	{
		var strProjTemplatePath = wizard.FindSymbol('PROJECT_TEMPLATE_PATH');
		var strProjTemplate = '';
		strProjTemplate = strProjTemplatePath + '\\default.vcproj';

		var Solution = dte.Solution;
		var strSolutionName = "";
		if (wizard.FindSymbol("CLOSE_SOLUTION"))
		{
			Solution.Close();
			strSolutionName = wizard.FindSymbol("VS_SOLUTION_NAME");
			if (strSolutionName.length)
			{
				var strSolutionPath = strProjectPath.substr(0, strProjectPath.length - strProjectName.length);
				Solution.Create(strSolutionPath, strSolutionName);
			}
		}

		var strProjectNameWithExt = '';
		strProjectNameWithExt = strProjectName + '.vcproj';

		var oTarget = wizard.FindSymbol("TARGET");
		var prj;
		if (wizard.FindSymbol("WIZARD_TYPE") == vsWizardAddSubProject)  // vsWizardAddSubProject
		{
			var prjItem = oTarget.AddFromTemplate(strProjTemplate, strProjectNameWithExt);
			prj = prjItem.SubProject;
		}
		else
		{
			prj = oTarget.AddFromTemplate(strProjTemplate, strProjectPath, strProjectNameWithExt);
		}
		return prj;
	}
	catch(e)
	{
		throw e;
	}
}

function AddFilters(proj)
{
	try
	{
		// Add the folders to your project
		var group = proj.Object.AddFilter('Source Files');
		group.Filter = "cpp;c;cxx;def;odl;idl;hpj;bat;asm";
		
		var group = proj.Object.AddFilter('Header Files');
		group.Filter = "h;hpp;hxx;hm;inl;inc";
		
		var group = proj.Object.AddFilter('Resource files');
		group.Filter = "rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe";		
	}
	catch(e)
	{
		throw e;
	}
}

function AddConfig(proj, strProjectName)
{
	try
	{
		var bCommonFramework = wizard.FindSymbol('FRAMEWORK_YES');
		var bPostbuildCopy = wizard.FindSymbol('POSTBUILD_COPY');
		var bUseCEGUI = wizard.FindSymbol('CEGUI_YES');

		var strOgreMainDir = '';
		strOgreMainDir = '$(OGRE_HOME)';

		var strCEGUIRendererDir = '';
		strCEGUIRendererDir = '$(OGRE_HOME)' + '\\samples\\include';

//DEBUG//////////////////////////////////////////////////////
		var config = proj.Object.Configurations('Debug');
		config.IntermediateDirectory = '..\\obj\\$(ConfigurationName)';
		config.OutputDirectory = '..\\bin\\$(ConfigurationName)';
		config.CharacterSet = charSet.charSetMBCS;
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
		var CLTool = config.Tools('VCCLCompilerTool');
		CLTool.RuntimeLibrary = rtMultiThreadedDebugDLL;
		CLTool.MinimalRebuild = true;
		CLTool.DebugInformationFormat = debugEditAndContinue;
		CLTool.BasicRuntimeChecks = runtimeBasicCheckAll;
		CLTool.Optimization = optimizeDisabled;
		CLTool.ExceptionHandling = true;

		CLTool.Detect64BitPortabilityProblems = true;
		CLTool.EnableFunctionLevelLinking = true;
		CLTool.MinimalRebuild = true;
		CLTool.ImproveFloatingPointConsistency = true;		
		CLTool.WarningLevel = warningLevelOption.warningLevel_3;
		CLTool.PreprocessorDefinitions = "WIN32;_DEBUG;_WINDOWS";


//		if(wizard.FindSymbol("PRE_COMPILED_HEADER"))
//			CLTool.UsePrecompiledHeader = pchGenerateAuto;
		if (wizard.FindSymbol("EMPTY_PROJECT"))
			CLTool.UsePrecompiledHeader = pchNone;
		
		var strAdditionalIncludeDirectories = '';
		strAdditionalIncludeDirectories = "..\\include;" + strOgreMainDir + "\\include";
		
		if(bUseCEGUI)
			strAdditionalIncludeDirectories = strAdditionalIncludeDirectories + ";" + strOgreMainDir + "\\include\\CEGUI" + ";" + strCEGUIRendererDir;

		CLTool.AdditionalIncludeDirectories = strAdditionalIncludeDirectories;
		
		var LinkTool = config.Tools('VCLinkerTool');
		LinkTool.GenerateDebugInformation = true;
		LinkTool.ProgramDatabaseFile = "$(OutDir)/" + strProjectName + ".pdb";
		LinkTool.LinkIncremental = linkIncrementalType.linkIncrementalYes;
		LinkTool.OptimizeForWindows98 = optWin98Type.optWin98No;
		LinkTool.SubSystem = subSystemOption.subSystemWindows;
		LinkTool.TargetMachine = machineTypeOption.machineX86;

		var strAdditionalLibraryDirectories = '';
		var strAdditionalDependencies = '';
		//\\$(ConfigurationName)
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

			PostBuildTool.CommandLine = "copy $(OutDir)\\$(TargetFileName) " + strOgreMainDir + "\\Bin\\$(ConfigurationName)";
		}

//RELEASE//////////////////////////////////////////////////////
		config = proj.Object.Configurations('Release');
		config.IntermediateDirectory = '..\\obj\\$(ConfigurationName)';
		config.OutputDirectory = '..\\bin\\$(ConfigurationName)';
		config.CharacterSet = charSet.charSetMBCS;


		var CLTool = config.Tools('VCCLCompilerTool');
		CLTool.RuntimeLibrary = rtMultiThreadedDLL;
		CLTool.ExceptionHandling = true;
		CLTool.DebugInformationFormat = debugEnabled;
		CLTool.WarningLevel = warningLevelOption.warningLevel_3;
		CLTool.ImproveFloatingPointConsistency = true;		
		CLTool.Detect64BitPortabilityProblems = true;
		CLTool.PreprocessorDefinitions = "WIN32;NDEBUG;_WINDOWS";
		CLTool.EnableFunctionLevelLinking = true;
		CLTool.MinimalRebuild = true;
		CLTool.FavorSizeOrSpeed = favorSizeOrSpeedOption.favorSpeed;
		CLTool.GlobalOptimizations = true;
		CLTool.InlineFunctionExpansion = inlineExpansionOption.expandOnlyInline;
		CLTool.Optimization = optimizeOption.optimizeMaxSpeed;
		CLTool.OmitFramePointers = true;
		CLTool.OptimizeForProcessor = ProcessorOptimizeOption.procOptimizePentiumProAndAbove;
		CLTool.OptimizeForWindowsApplication = true;
		CLTool.StringPooling = true;

//		if(wizard.FindSymbol("PRE_COMPILED_HEADER"))
//			CLTool.UsePrecompiledHeader = pchGenerateAuto;
		if (wizard.FindSymbol("EMPTY_PROJECT"))
			CLTool.UsePrecompiledHeader = pchNone;

		var strAdditionalIncludeDirectories = '';
		strAdditionalIncludeDirectories = "..\\include;" + strOgreMainDir + "\\include";
		
		if(bUseCEGUI)
			strAdditionalIncludeDirectories = strAdditionalIncludeDirectories + ";" + strOgreMainDir + "\\include\\CEGUI" + ";" + strCEGUIRendererDir;
		
		CLTool.AdditionalIncludeDirectories = strAdditionalIncludeDirectories;
		
		var LinkTool = config.Tools('VCLinkerTool');
		LinkTool.LinkIncremental = linkIncrementalType.linkIncrementalNo;
		LinkTool.OptimizeForWindows98 = optWin98Type.optWin98No;
		LinkTool.SubSystem = subSystemOption.subSystemWindows;
		LinkTool.TargetMachine = machineTypeOption.machineX86;
		LinkTool.EnableCOMDATFolding = optFoldingType.optFolding;
		LinkTool.OptimizeReferences = optRefType.optReferences;

		//\\$(ConfigurationName)
		var strAdditionalLibraryDirectories = '';
		strAdditionalLibraryDirectories = strOgreMainDir + "\\lib";
		
		var strAdditionalDependencies = '';
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

			PostBuildTool.CommandLine = "copy $(OutDir)\\$(TargetFileName) " + strOgreMainDir + "\\Bin\\$(ConfigurationName)";
		}
	}
	catch(e)
	{
		throw e;
	}
}

function PchSettings(proj)
{
	var bPch = wizard.FindSymbol("PRE_COMPILED_HEADER");
	if(!bPch)
	{
		SetNoPchSettings(proj);
	}
	else
	{
		SetCommonPchSettings(proj);
	}	
}

function DelFile(fso, strWizTempFile)
{
	try
	{
		if (fso.FileExists(strWizTempFile))
		{
			var tmpFile = fso.GetFile(strWizTempFile);
			tmpFile.Delete();
		}
	}
	catch(e)
	{
		throw e;
	}
}

function CreateCustomInfFile()
{
	try
	{
		var fso, TemplatesFolder, TemplateFiles, strTemplate;
		fso = new ActiveXObject('Scripting.FileSystemObject');

		var TemporaryFolder = 2;
		var tfolder = fso.GetSpecialFolder(TemporaryFolder);
		var strTempFolder = tfolder.Drive + '\\' + tfolder.Name;

		var strWizTempFile = strTempFolder + "\\" + fso.GetTempName();

		var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');
		var strInfFile = strTemplatePath + '\\Templates.inf';
		wizard.RenderTemplate(strInfFile, strWizTempFile);

		var WizTempFile = fso.GetFile(strWizTempFile);
		return WizTempFile;
	}
	catch(e)
	{
		throw e;
	}
}

function GetTargetName(strName, strProjectName)
{
	try
	{
		// TODO: set the name of the rendered file based on the template filename
		var strTarget = strName;

		if (strName == '..\\include\\main.h')
			strTarget = '..\\include\\' + strProjectName + '.h'

		if (strName == 'res\\base_sample.rc')
			strTarget = 'res\\' + strProjectName + '.rc'

		return strTarget; 
	}
	catch(e)
	{
		throw e;
	}
}

function AddFilesToCustomProj(proj, strProjectName, strProjectPath, InfFile)
{
	try
	{
		var projItems = proj.ProjectItems

		var strTemplatePath = wizard.FindSymbol('TEMPLATES_PATH');

		var strTpl = '';
		var strName = '';

		var strTextStream = InfFile.OpenAsTextStream(1, -2);
		while (!strTextStream.AtEndOfStream)
		{
			strTpl = strTextStream.ReadLine();
			if (strTpl != '')
			{
				strName = strTpl;
				var strTarget = GetTargetName(strName, strProjectName);
				var strTemplate = strTemplatePath + '\\' + strTpl;
				var strFile = strProjectPath + '\\' + strTarget;

				var bCopyOnly = false;  //"true" will only copy the file from strTemplate to strTarget without rendering/adding to the project
				var strExt = strName.substr(strName.lastIndexOf("."));
				if(strExt==".bmp" || strExt==".ico" || strExt==".gif" || strExt==".rtf" || strExt==".css")
					bCopyOnly = true;
				wizard.RenderTemplate(strTemplate, strFile, bCopyOnly);
				proj.Object.AddFile(strFile);
			}
		}
		strTextStream.Close();
	}
	catch(e)
	{
		throw e;
	}
}
