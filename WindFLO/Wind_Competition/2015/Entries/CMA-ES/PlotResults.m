close all;
clear all;

results_dir = 'testresults/';

% assumptions for graphs: the same number of function evaluation performed for all
% runs, e.g., 1000
Runs = [1:3]; %number of runs
problems_idx = [1:10];  % select indexed from 'problems' list of problems
modes = [2]; % 1 - mode for fixed number of turbines, 2 - for dynamic number of turbines

markers = {'+','o','*','.','x','s','d','^','v','p','h','>','<','+','o','*','.','x','s','d','^','v','p','h','>','<'};
markers = [markers markers markers];
markers = [markers markers markers];
markers = [markers markers markers];
markers = [markers markers markers];
plotit = 2; % 0 - don't plot, 1 - plot convergence for each run on each function, 2 - medians for each function
linsemilog = 0; % 0 - linear scaling on x, 1 - logx

for mode=modes % 1 - mode for fixed number of turbines, 2 - for dynamic number of turbines
    legend_itr = 0;
  	for iproblem=problems_idx-1
        run_itr = 0;
        results = 0;
        for iRun=Runs
            run_itr = run_itr + 1;
            subdir = ['run' num2str(iRun)];
            fulldir = [results_dir subdir '/']; % directory for the run
            modedirname = ['mode' num2str(mode)];
            results_dir_mode = [fulldir modedirname '/'];
      
            
            solutions_filename = [results_dir_mode 'allsolutions_problem' num2str(iproblem) '.txt'];
            fitness_filename = [results_dir_mode 'onlyfitness_problem' num2str(iproblem) '.txt'];

            M = dlmread(fitness_filename);
            ieval = M(:,1);
            best = M(:,2);

            color = [rand() rand() rand()];
            %szhere = numel(problems_idx);
            %color = [legend_itr/szhere 1-itr/szhere itr/szhere];
            
            %plot(ieval,best,marker,'color',color);    hold on;
            if (plotit == 1)
                legend_itr = legend_itr + 1;
                marker = ['-' markers{legend_itr}];
                legend_names{legend_itr} = ['p' num2str(iproblem) ':' num2str(max(best))];
                if (linsemilog == 0)	plot(ieval,best,marker,'color',color);      hold on;        end;
                if (linsemilog == 1)	semilogx(ieval,best,marker,'color',color);	hold on;        end;                
            end;            
            results(iRun,1:numel(best)) = best(1:numel(best));
        end;
      %  med_bestfit = median(results(:,end));
        med_results = median(results(:,:));
        med_bestfit = med_results(end);
        if (plotit == 2)
            legend_itr = legend_itr + 1;
            marker = ['-' markers{legend_itr}];
            legend_names{legend_itr} = ['p' num2str(iproblem) ':' num2str(max(best))];
            if (linsemilog == 0)	plot(ieval,med_results,marker,'color',color);      hold on;        end;
            if (linsemilog == 1)	semilogx(ieval,med_results,marker,'color',color);	hold on;        end;
        end;
        str = ['median for best result of problem' num2str(iproblem) ' : ' num2str(med_bestfit)];
        disp(str);
        if (plotit > 0)
            if (plotit == 1)            str = ['performance for each run'];  end;
            if (plotit == 2)            str = ['medians for ' num2str(numel(Runs)) ' are shown'];   end;
            title(str);
        end;
    end;
end;
if (plotit > 0)
    xlabel('Number of evaluations');
    ylabel('Objective function');
    legend(legend_names,'Location','BestOutside');
end;
z = 0;