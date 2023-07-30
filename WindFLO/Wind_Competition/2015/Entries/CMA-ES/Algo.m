function [bestFit, bestSolutionReturn] = Algo(wfle,nevalsmax,N,mode,plotit, wakeration_min)

global gSettings;

close all;
x = -10; y = 0; w = 500; h = 700;
if (plotit == 1)
    set(0,'defaultfigureposition',[x y w h]');
    colorbar;
end;
itr = 0;
bestFit = 0;

nobstacles = size(wfle.ws.obstacles,1);

%%% for mode2
N0 = N;
nokTurbines_best_ever = 0;
minRadius = 8.001 * wfle.ws.R;
Nmax_possible = (1 + floor(wfle.ws.width/minRadius)) * (1 + floor(wfle.ws.height/minRadius));
Nfactor = 1.1;
NshrinkingFactor = 0.5;
mode2_localreached = 0;
TurbineFitnesses_best_ever = 0;
h1 = 777;   h2 = 777;
%%%

%%% mode1
mode1_Nadd = 0;
%%%

stop_mode2 = 0;
itr2_best = -1;
itr2 = 0;
removeType_undef = 1;
global settings;
global hyperparams;
hyperparams.removeType = 0; 
while ((stop_mode2 == 0) && (wfle.nEvals < nevalsmax))

if (mode2_localreached == 1)
    N0 = nokTurbines_best_ever + 1;
end;
N = N0;
nokTurbines_best = 0;

itr2 = itr2 + 1;
devv1 = 0.5;
devv2 = devv1 - 0.1 + 0.2*rand();
ieval = 0;
stop = 0;

% delete node
% 


while ((stop == 0) && (wfle.nEvals < nevalsmax))
    ieval = ieval + 1;
    itr = itr + 1;
    
    
    N_trial = floor(N+mode1_Nadd);%floor(N/(1 - perc_obstacles)) + 1;
    
    Nfeasible_found = 0;
    solution = 0;
  	epshere = 0.00001;
    
    
    if (mode == 2)
        hyperparams.removeType = 0;   
        x0 = epshere;   x1 = 1.0-epshere;
     	y0 = epshere;   y1 = 1.0-epshere;
        [solution, stop, nx, ny] = fill_subspace(Nfeasible_found, N, x0, x1, y0, y1, wfle.ws.width, wfle.ws.height, ...
                                                minRadius, N_trial, ieval, nobstacles, wfle.ws.obstacles);
    end;
    if (mode == 1)
        if (itr2 == 1)  
            hyperparams.removeType = 0;     
        end;
        if (itr2 == 2)  
            hyperparams.removeType = 1;     
        end;
        if (itr2 == 3)  
            hyperparams.removeType = 2;     
        end;
        if (itr2 == 4)
            if (removeType_undef == 1)
                hyperparams.removeType = itr2_best - 1;
                removeType_undef = 0;
            end;
        end;
     %   if (itr2 > 3)
      %      hyperparams.removeType = floor(rand()*3);
      %  end;
        if (1)%itr2 == -1)
      %  disp('1');
            x0 = epshere;   x1 = 1.0-epshere;
            y0 = epshere;   y1 = 1.0-epshere;
            Nmaxpossible = floor( (x1 - x0)*(y1 - y0)*wfle.ws.width*wfle.ws.height / (minRadius*minRadius) );
            if (N > Nmaxpossible)
                N = Nmaxpossible-1;
                disp('warning Nmaxpossible');
            end;

            if ((itr2 > 5)&&(mode == 1)) % improve the best solution
                ieval = ieval_best;
                solution = solution_best;
               % wfle = wfle.evaluate(solution);
              %  if (plotit == 1)    [h1, h2] = plotsurf(solution,wfle.TurbineFitnesses,wfle.ws.width,wfle.ws.height,...
              %                      h1,h2,itr,bestFit,targetF,N,wfle.wfRatio,wfle.EnergyOutput,nokTurbines_best_ever,mode);     end;

               % wfle = wfle.evaluate(solution);
               if (wfle.nEvals < settings.evals0)%hyperparams.mutationType <= 3)
                   [solution, stop, nx, ny] = fill_subspace(Nfeasible_found, N, x0, x1, y0, y1, wfle.ws.width, wfle.ws.height, ...
                                                        minRadius, N_trial, ieval, nobstacles, wfle.ws.obstacles);

    %                disp('2');
                                                    %end;
               %if (hyperparams.mutationType == 4)
               else
     %              disp('3');
                   if (0)
                        [xy(1,1),xy(1,2)] = ginput(1)
                        soli = repmat(xy,N,1);
                        dist = sqrt( sum( ((soli - solution(1:N,:)).^2)' ) );
                        [minval imin] = min(dist);
                        ddev = 10;
                        solution(imin,:) = solution(imin,:) + randn(1,2)*ddev;%xy(1,:);
                        solution(imin,:) = xy(1,:);
                   else
                        solution_tmp = solution;
                        feasible = 0;

                        settings.Nrepeats = floor(N/10);
                        itrial = 0;
                        while (feasible == 0)
                            itrial = itrial + 1;
                            if (itrial > 100)   
                                disp('breal trial');
                                break;  
                            end;
                            solution_tmp = solution;
                            for itry=1:settings.Nrepeats
                                xy(itry,1) = rand()*wfle.ws.width;
                                xy(itry,2) = rand()*wfle.ws.height;
                                soli = repmat(xy(itry,:),N,1);
                                dist = sqrt( sum( ((soli - solution_tmp(1:N,:)).^2)' ) );
                                [minval(itry) imin(itry)] = min(dist);

                                if (0)
                                    [distmin iarr] = sort(dist,'descend');
                                    nnsz = 3;
                                    estfit = 0;
                                    for nn=1:nnsz
                                        estfit = estfit + TurbineFitnessesBest(iarr(nn));                            
                                    end;
                                    estfit = estfit/3;
                                    estfitarr(itry) = estfit;
                                end;
                             %   ddev = 10;
                             %   dx =  randn(1,2)*ddev;%xy(1,:);
                             %   solution_tmp(imin,:) = solution_tmp(imin,:) + dx(1,:);
                             %   if (solution_tmp(imin,1) < 0)   solution_tmp(imin,1) = 0;   end;
                             %   if (solution_tmp(imin,1) > wfle.ws.width)   solution_tmp(imin,1) = wfle.ws.width;   end;
                             %   if (solution_tmp(imin,2) < 0)   solution_tmp(imin,2) = 0;   end;
                              %  if (solution_tmp(imin,2) > wfle.ws.height)   solution_tmp(imin,2) = wfle.ws.height;   end;
                             %   solution_tmp(imin(itry),:) = xy(1,:);
                            end;
                        %    [minval_srt iarr] = sort(minval,'descend');
                            [maxval imax] = max(minval);
                           % [maxval imax] = min(estfitarr);


                                soli = repmat(xy(imax,:),N,1);
                                dist = sqrt( sum( ((soli - solution_tmp(1:N,:)).^2)' ) );
                                [dst_srt iarr] = sort(dist,'ascend');

                            shiftcoef = 1;
                            ktomove = 1;
                            for ii=1:ktomove
                                solution_tmp(iarr(ii),:) = ...
                                    solution_tmp(iarr(ii),:)*(1 - shiftcoef) + shiftcoef * xy(imax,:);
                            end;
                            feasible = checkFeasible(solution_tmp, wfle.ws.width, wfle.ws.height, minRadius);
                        end;
       %                 disp('4');
                        solution = solution_tmp;
                   end;
        %           disp('5');
               end;
            else
                [solution, stop, nx, ny] = fill_subspace(Nfeasible_found, N, x0, x1, y0, y1, wfle.ws.width, wfle.ws.height, ...
                                                        minRadius, N_trial, ieval, nobstacles, wfle.ws.obstacles);
            end;
        end;
    end;
  %  disp(num2str(wfle.nEvals));
    if (numel(solution)/2 > N)
        disp('N of solution > Ntarget');
    end;
    if (checkWholeSolutionObstacles(solution, nobstacles, wfle.ws.obstacles) == 0)  % infeasible
        continue;
    else
       wfle = wfle.evaluate(solution);
    end;
 %   disp([num2str(wfle.nEvals) ' nx:' num2str(nx) ' ny:' num2str(ny) ' ' num2str(wfle.wfRatio)]);
    if (wfle.wfRatio <= 0)
        disp(['!wfle:' num2str(wfle.wfRatio)]);
    end;
    curFit = wfle.wfRatio;
    if (mode == 2)  
        curFit = -1;
        if (numel( find(wfle.TurbineFitnesses>wakeration_min))  == numel(wfle.TurbineFitnesses))
            curFit = numel(wfle.TurbineFitnesses);
        end;
    end;
    if (curFit > bestFit)
        bestFit = curFit;
        nx_best = nx;
        ny_best = ny;
        ieval_best = ieval;
        solution_best = solution;
        TurbineFitnessesBest = wfle.TurbineFitnesses;
        itr2_best = itr2;
    end;
  %  disp([num2str(wfle.nEvals) ' ' num2str(wfle.wfRatio) ' ' num2str(N_trial)]);
    
    if (mode == 1)
        fprintf(gSettings.gfile,'%d %g %g %g',wfle.nEvals,bestFit,wfle.wfRatio, wfle.EnergyOutput);    
        for k=1:N
            fprintf(gSettings.gfile,' %g %g',solution(k,1),solution(k,2));
        end;
        for k=1:N
            fprintf(gSettings.gfile,' %g',wfle.TurbineFitnesses(k));
        end;
        fprintf(gSettings.gfile,'\n');
        
        fprintf(gSettings.gfile2,'%d %g %g %g\n',wfle.nEvals,bestFit,wfle.wfRatio, wfle.EnergyOutput);    
    end;
    
  %  disp([num2str(ieval) ' ' num2str(bestFit) ' ' num2str(wfle.EnergyOutput)]);
    
    if (plotit == 1)    [h1, h2] = plotsurf(solution,wfle.TurbineFitnesses,wfle.ws.width,wfle.ws.height,...
                                h1,h2,itr,bestFit,N,wfle.wfRatio,wfle.EnergyOutput,nokTurbines_best_ever,mode);     end;
                        
    if (mode == 2)
        nokTurbines = numel( find(wfle.TurbineFitnesses>wakeration_min) );
        if (nokTurbines > nokTurbines_best)
            nokTurbines_best = nokTurbines;
            solution_best = solution;
            TurbineFitnesses_best = wfle.TurbineFitnesses;
            wfRatio_of_best = wfle.wfRatio;
            EnergyOutput_of_best = wfle.EnergyOutput;
        end;
        
        if (nokTurbines == N)
            stop = 1;
        end;
    else
        stop_mode2 = 1;
    end;
   % return;
end;

if (mode == 2)
    if (nokTurbines_best > nokTurbines_best_ever)
        TurbineFitnesses_best_ever = TurbineFitnesses_best;
        nokTurbines_best_ever = nokTurbines_best;
        solution_best_ever = solution_best;
        wfRatio_of_best_ever = wfRatio_of_best;
      	EnergyOutput_of_best_ever = EnergyOutput_of_best;
    end;

   	fprintf(gSettings.gfile2,'%d %g %g %g\n',wfle.nEvals,nokTurbines_best_ever,wfRatio_of_best_ever, EnergyOutput_of_best);    

  	if ((nokTurbines_best == N))% all are ok
        N0 = floor(N * Nfactor); 
    else % no, some are < wakeration_min
        %Nfactor = 1.0 + (Nfactor - 1.0) * NshrinkingFactor;
        N0 = N/Nfactor;
        Nfactor = 1.0 + (Nfactor - 1.0) * NshrinkingFactor;
        if (abs(N0 - floor(N0 * Nfactor)) < 1)
            %stop_mode2 = 1;
            mode2_localreached = 1;
        end;
        N0 = floor(N0 * Nfactor); 
    end;
end;
    if (mode == 2)
        bestFit = nokTurbines_best_ever;
    end;
    if (mode == 1)
        stop_mode2 = 0;
     %   mode1_Nadd = mode1_Nadd + 1;
    end;
    if (N > 0.9*Nmax_possible)
        stop_mode2 = 1;
    end;
end;

if (mode == 2)
    N = numel(TurbineFitnesses_best_ever);
    fprintf(gSettings.gfile2,'%d %g %g %g',wfle.nEvals,N,wfRatio_of_best_ever, EnergyOutput_of_best_ever);
    for k=1:N
        fprintf(gSettings.gfile2,' %g %g',solution_best_ever(k,1),solution_best_ever(k,2));
    end;
    for k=1:N
        fprintf(gSettings.gfile2,' %g',TurbineFitnesses_best_ever(k));
    end;    
  	fprintf(gSettings.gfile2,'\n');
end;

if (mode == 1)    bestSolutionReturn = solution_best;           end;
if (mode == 2)    bestSolutionReturn = solution_best_ever;      end;  % spaghetti notation ;-) 


function [solution, stop, nx, ny] = fill_subspace(Nfeasible_found, N, x0, x1, y0, y1, width, height, ...
    minRadius, N_trial, ieval, nobstacles, obstacles)

stop = 0;
itrial = 0;
while (Nfeasible_found < N)
        itrial = itrial + 1;
        if (itrial > 1000)  
            disp('break trial2');
            break;  
        end;
        nx_max = floor( ((x1 - x0)*width  / minRadius)) + 1;    
        ny_max = floor( ((y1 - y0)*height / minRadius)) + 1;
        nx_min = floor(N_trial/ny_max) + 1;
        ny_min = floor(N_trial/nx_max) + 1;
        if (nx_min <= 1)    nx_min = 2; end;
        if (ny_min <= 1)    ny_min = 2; end;
       	if (nx_max <= 1)    nx_max = 2; end;
        if (ny_max <= 1)    ny_max = 2; end;
        if (1)
            if (ieval == 1)
                nx = nx_min;
                ny = floor(N_trial/nx) + 1;
            end;
            if (ieval == 2)
                ny = ny_min;
                nx = floor(N_trial/ny) + 1;
            end;
            if (ieval > 2)
                if (nx_max > ny_max)%wfle.ws.width < wfle.ws.height)
                    nx = nx_min + ieval - 3;
                    if (nx > nx_max)    
                        nx = nx_max;  
                %        disp('nx = nx_max reached');  
                    end;
                    if (nx == nx_max)   
                        stop = 1;   
                    end;
                    ny = floor(N_trial/nx) + 1;
                else
                    ny = ny_min + ieval - 3;
                    if (ny > ny_max)    
                        ny = ny_max;  
                  %      disp('ny = ny_max reached');  
                    end;
                    if (ny == ny_max)   
                        stop = 1;   
                    end;
                    nx = floor(N_trial/ny) + 1;
                end;
            end;
            if (nx <= 1)    nx = 2; end;
            if (ny <= 1)    ny = 2; end;
            
            Nfeasible = 0;
            ix = -1;
            if (1)
                Norig = N;
                N_here = nx * ny;
                if (N_trial < N_here)    N_trial = N_here; end;
            end;
            while ((ix < N_trial-1) && (Nfeasible < N_here))
                ix = ix + 1;
           % for ix=1:N_here
                ixmod = rem(ix,nx);
                iymod = floor((ix/nx));%(ix - ixmod)/np;

                
                xcur = ((x0 + (x1 - x0)*(ixmod)*(1/(nx-1))) )*width;
                ycur = ((y0 + (y1 - y0)*(iymod)*(1/(ny-1))) )*height;
               % if ((xcur*0 ~= 0) || (ycur*0 ~= 0))
               %     zzz = 0;
              %  end;

                isFeasible = checkObstacles(xcur, ycur, nobstacles, obstacles);
                if (isFeasible == 1)
                    Nfeasible = Nfeasible + 1;
                    solution(Nfeasible,1) = xcur;
                    solution(Nfeasible,2) = ycur;
                end;
            end;
            if (1)
                Ntodelete = Nfeasible-Norig;
                for k=1:Ntodelete
                    feasible = 0;
                    itrial2 = 0;
                    while(feasible == 0)
                        itrial2 = itrial2 + 1;
                        if (itrial2 > 100)   disp('break 3');    break;  end;
                        indx = 1 + floor( rand()* (Nfeasible+1-k) );
                        solution_tmp = solution;
                        solution_tmp = removeSolution(solution_tmp, indx, x0, x1, y0, y1, width, height);
                        feasible = checkFeasible(solution_tmp, width, height, minRadius);
                        if (feasible == 1)
                            feasible = checkWholeSolutionObstacles(solution_tmp, nobstacles, obstacles);
                        end;
                    end;
                    solution = solution_tmp;
                    %solution(indx,:) = [];
                end;
                if (Ntodelete > 0)
                    Nfeasible = Nfeasible - Ntodelete;
                end;
            end;
            Nfeasible_found = Nfeasible;
        end;
         N_trial =  N_trial + 1;
    end;
    
function feasible = checkFeasible(solution, width, height, minRadius)

feasible = 1;
N = numel(solution)/2;
if (0) % slow
    distmin = 1e+10;
    for i=1:N
        j = (i+1):N;
        soli = repmat(solution(i,:),numel(j),1);
            dist = sqrt( sum( ((soli - solution(j,:)).^2)' ) );
            %ifind = find(dist < minRadius);
            distminhere = min(dist);
            if (dist < distmin)
                distmin = distminhere;
            end;
         %   if (numel(ifind) > 0)
         %       feasible = 0;
         %       return;
         %   end;

    end;
end;

if (1) % fast
    p = solution;
   [x y]=meshgrid(1:N);
    i=find(triu(ones(N)-eye(N)));
    i=[x(i) y(i)];

    % compute all pairwise distances
    d2=(p(i(:,1),1)-p(i(:,2),1)).^2 + ...
       (p(i(:,1),2)-p(i(:,2),2)).^2;

    % find minimum distance and corresponding points
    [d2min imin]=min(d2);
    if (sqrt(d2min) < minRadius)
        feasible = 0;
    else
        feasible = 1;
    end;
end;

function isFeasible = checkObstacles(xcur, ycur, nobstacles, obstacles)
isFeasible = 1;
for k=1:nobstacles
                    x_min = obstacles(k,1);    x_max = obstacles(k,2);
                    y_min = obstacles(k,3);    y_max = obstacles(k,4);
                    if ((xcur >= x_min) && (xcur <= x_max) && (ycur >= y_min) && (ycur <= y_max))
                        isFeasible = 0;
                    end;
                end;
                
function isFeasible = checkWholeSolutionObstacles(solution, nobstacles, obstacles)

N = numel(solution)/2;
isFeasible = 1;
for i=1:N
    if (checkObstacles(solution(i,1), solution(i,2), nobstacles, obstacles) == 0)
        isFeasible = 0;
        return;
    end;
end;

function solution = removeSolution(solution, indx, x0, x1, y0, y1, width, height)
    
global hyperparams;
if (hyperparams.removeType == 0)
    solution(indx,:) = [];
else
   % solution = solution_best;
    %    solution_tmp = solution;
    %    bestColFit = 0;
    %    bestColIndx = 0;
    %    while( numel(solution_tmp) > 0)
    
    
    if (hyperparams.removeType == 1)%(rand()<0.5)
            xcur = solution(indx,1);
            indx_col = find(solution(:,1) == xcur);
            nremoved = numel(indx_col);
            solution(indx_col,:) = [];
            ny = nremoved - 1;
            
            if (ny == 1)
                pt(1,1) = xcur;
                if (rand() < 0.5)   pt(1,2) = y0;
                else                pt(1,2) = y1;   end;
                solution = [solution; pt];
            else
                for iy = 1:ny
                    pt(1,1) = xcur;
                    pt(1,2) = ((y0 + (y1 - y0)*(iy-1)*(1/(ny-1))) )*height;
                    solution = [solution; pt];
                end;
            end;
    end;
    if (hyperparams.removeType == 2)
            ycur = solution(indx,2);
            indx_row = find(solution(:,2) == ycur);
            nremoved = numel(indx_row);
            solution(indx_row,:) = [];
            nx = nremoved - 1;
            
             if (nx == 1)
                pt(1,2) = ycur;
                if (rand() < 0.5)   pt(1,1) = x0;
                else                pt(1,1) = x1;   end;
                solution = [solution; pt];
            else
                for ix = 1:nx
                    pt(1,1) = ((x0 + (x1 - x0)*(ix-1)*(1/(nx-1))) )*width;
                    pt(1,2) = ycur;
                    solution = [solution; pt];
                end;
             end;
    end;
    
           % indx_col = find(solution(:,1) == xcur);
           % curColFit = mean(wfle.TurbineFitnesses(indx_col));
           %if ((curColFit > bestColFit)&&(numel(indx_col) > 3))
           %     bestColFit = curColFit;
          %      bestColIndx = indx_col;
           % end;
    %    end;

end;
    
function [h1, h2] = plotsurf(solution,TurbineFitnesses,width,height,h1,h2,itr,bestFit,...
    N,wfRatio,EnergyOutput,nokTurbines_best_ever,mode)
        
        xy = solution;
        f = TurbineFitnesses;
        
        minx = 0;   maxx = width;
        miny = 0;   maxy = height;
        ylin = linspace(miny,maxy,100);
        xlin = linspace(minx,maxx,100);
        [X,Y] = meshgrid(xlin,ylin);

        if (itr > 1)
            delete(h1);
            delete(h2);
        end;
    	color = 'blue';
        if (1)
            hold off;
            z = f(:);        Z = griddata(xy(:,1),xy(:,2),z,X,Y,'linear');   hold on;%cubic');%'nearest');'linear'
            h1 = plot3(xy(:,1),xy(:,2),z,'.','color',color);
        else
            z = f_all(:);        Z = griddata(xy_all(:,1),xy_all(:,2),z,X,Y,'linear');   hold on;%cubic');%'nearest');'linear'
        end;
        MyColor = Z;
        h2 = surf(X,Y,Z,MyColor, 'EdgeColor', 'none');
       % dperc = 100.0*(bestFit-targetF)/targetF;
        if (mode == 1)
            title(['N=' num2str(N) ' ' num2str(wfRatio) '% ' num2str(EnergyOutput)]);
        end;
        if (mode == 2)
            title(['N=' num2str(N) ' Nbest=' num2str(nokTurbines_best_ever)]);
        end;
        drawnow
