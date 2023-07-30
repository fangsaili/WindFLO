
close all;

scenario_dir = 'f:/GECCOcomp/WindFLO-master/Scenarios/';    % folder with scenarion, choose yours
problems = {'00.xml','01.xml','02.xml','03.xml','04.xml',...
            '05.xml','06.xml','07.xml','08.xml','09.xml',...
            'obs_00.xml','obs_01.xml','obs_02.xml','obs_03.xml','obs_04.xml',...
            'obs_05.xml','obs_06.xml','obs_07.xml','obs_08.xml','obs_09.xml'};  % names of scenario if the directory
        
results_dir = 'testresults/';   % destination directy
if (exist(results_dir,'dir') ~= 7)
    mkdir(results_dir); % make it
end;
global settings;
Runs = [1:3]; %runs, e.g. [1,2] or [1:15]
problems_idx = [1:20];  % select indexed from 'problems' list of problems
modes = [1:2]; % 1 - mode for fixed number of turbines, 2 - for dynamic number of turbines
pritbestsolution = 0;   % 0 - don't, 1 - do
nevalsmaxPerProblem = 50; 
%%%%%%%%%%%%%% TO  SETUP YOUR PROBLEMS %%%%%%%%%%%%%%%%%%
if (0)
    % 1. Choose the track
    modes = 1; % 1 - mode for fixed number of turbines, 2 - for dynamic number of turbines
    % 2. Run it several times
    Runs = [1:3];
    % 3. Set-up problems 
    problems = {'00.xml','01.xml','02.xml','03.xml','04.xml',...
            '05.xml','06.xml','07.xml','08.xml','09.xml',...
            'obs_00.xml','obs_01.xml','obs_02.xml','obs_03.xml','obs_04.xml',...
            'obs_05.xml','obs_06.xml','obs_07.xml','obs_08.xml','obs_09.xml'};  % names of scenario if the directory
    % 4. Select indexes of problems
    problems_idx = [1:5];
    % 5. Select destination directory for the data
    results_dir = 'testresults/';
    % 6. Decide whether you the best solution found to be printed or not
    pritbestsolution = 0;
    % 7. Set maximum budget of evaluation per problem
    % For the competition: is the total budget is X (e.g., 5000) for Y
    % problems (e.g., 5), 
    % then nevalsmaxPerProblem = floor(X/Y) (e.g.,
    nevalsmaxPerProblem = 1000; 
    % 7. after the optimization is finished, 
    % check the console for the results and data files in 'results_dir'
    % you may configure and run PlotResults.m to make convergence plots
end;
%%%%%%%%%%%%%% TO  SETUP YOUR PROBLEMS %%%%%%%%%%%%%%%%%%

global gSettings;
plotit = 1; % 0 - don't plot, 1 - plot nice figure

tic
for mode=modes % 1 - mode for fixed number of turbines, 2 - for dynamic number of turbines
    if (mode == 1)	wakeration_min = 0.0;   end;
    if (mode == 2)  wakeration_min = 0.8;  end;
    disp(['Mode: ' num2str(mode)]);
    for iRun=Runs % run multiple time each time in new directory
        disp(['iRun: ' num2str(iRun)]);
        subdir = ['run' num2str(iRun)];
        if (exist([results_dir '/' subdir],'dir') ~= 7)
            mkdir(results_dir,subdir);  
        end;
        fulldir = [results_dir subdir '/']; % directory for the run

        settings.evals0 = 300;
    
        modedirname = ['mode' num2str(mode)];
        if (exist([fulldir '/' modedirname],'dir') ~= 7)
            mkdir(fulldir,modedirname);
        end;
        results_dir_mode = [fulldir modedirname '/'];
        for iproblem=problems_idx-1
           	xmlfilename = [scenario_dir problems{iproblem+1}];
            wfle = WindFarmLayoutEvaluator(xmlfilename);
            gSettings.gfile = fopen([results_dir_mode 'allsolutions_problem' num2str(iproblem) '.txt'],'w'); % only the last one for mode2
            gSettings.gfile2 = fopen([results_dir_mode 'onlyfitness_problem' num2str(iproblem) '.txt'],'w');
            gSettings.ievals = 0;

            N = wfle.ws.nturbines;

            if (mode == 2)  N = floor(N/2); end;    % a conservative start for mode2

            [bestFit, bestSolutionReturn] = Algo(wfle,nevalsmaxPerProblem,N,mode,plotit,wakeration_min);
            if (pritbestsolution == 1)
                bestSolutionReturn
            end;
            if (mode == 1)
                disp(['iproblem: ' num2str(iproblem) ' bestFit: ' num2str(bestFit)]);
            end;
            if (mode == 2)
                disp(['iproblem: ' num2str(iproblem) ' bestFit: ' num2str(bestFit)]);
            end;
            fclose(gSettings.gfile);
            fclose(gSettings.gfile2);
        end;
    end;
end;
toc