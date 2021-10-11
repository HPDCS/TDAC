%-----------------------------------%
%CONFIGURATION PARAMETERS

%number of operations per transaction
n=8;
%number of data items
d=128;
%max number of threads
max_threads=32;
%percentage of threads of class 2 (threads executing the second access pattern)
perc_threadsG2_run=50;
%expected accuracy
epsilon=0.001; 
%operation state residece times
T=ones(n,1);
%commit state residece times
tc=1;

%END CONFIGURATION PARAMETERS
%-----------------------------------%
fprintf("-----------------------------------------------------------------\n");
warning('off','all')
warning

print_avg_time_run_and_abort_prob=0;
fprintf("-----------------------------------------------------------------\n");
fprintf("Configuration: n=%i, d=%i, %%threadsG1=%i\n", n,d, 100-perc_threadsG2_run);
fprintf("-----------------------------------------------------------------\n");
fprintf("Response times (R) and throughput (Thr)\n");
fprintf("\n");
if (not(print_avg_time_run_and_abort_prob)) 
    fprintf("Threads\tR Case 1.1\tThr Case 1.1\tR Case 1.2\tThr Case 1.2\tR Case 1.3\tThr Case 1.3\tR Case 1.4\tThr Case 1.4\n");
else
    fprintf("Threads\tR Case 1.1\tThr Case 1.1\tRun Case 1.1\tAbort Case 1.1\tR Case 1.2\tThr Case 1.2\tRun Case 1.2\tAbort Case 1.2\tR Case 1.3\tThr Case 1.3\tRun Case 1.3\tAbort Case 1.3\tR Case 1.4\tThr Case 1.4\tRun Case 1.4\tAbort Case 1.4\n");
end
Throughput1=zeros(max_threads,1);
Throughput2=zeros(max_threads,1);
Throughput3=zeros(max_threads,1);
Throughput4=zeros(max_threads,1);

Abort_probability1=zeros(max_threads,1);
Abort_probability2=zeros(max_threads,1);
Abort_probability3=zeros(max_threads,1);
Abort_probability4=zeros(max_threads,1); 

threads=1;

p_rand=zeros(n,1);
p_incr=zeros(n,1);
p_G1_r1=zeros(n,1);
p_G2_r1=zeros(n,1);
p_G1_r2=zeros(n,1);
p_G2_r2=zeros(n,1);
R_prev=0;

s=d/n;

while threads<=max_threads   
    R=epsilon;
    fprintf("%i", threads);
    while (abs(R_prev-R)>=epsilon)
        R_prev=R;
        l=zeros(n,1);
        f=zeros(n,1);
        V=zeros(n,1);
        i=1;
        while i<=n
            k=i:n; V(i)=(-1)^(n-(i-1))*(1/(prod(p_rand(k)-1)));
            i=i+1;
        end
        R=dot(V,T)+tc;
        i=1;
        while i<=n
            k=i+1;
            while k<=n
                l(i)=l(i)+V(k)*T(k);
                k=k+1;
            end
            l(i)=l(i)+tc;
            i=i+1;
        end
        i=1;
        while i<=n
            f(i)=l(i)/R;
            i=i+1;
        end        
        i=1;
        f_avg_x=0;
        while i<=n
            f_avg_x=f_avg_x+f(i);
            i=i+1;
        end
        f_avg_x=f_avg_x/n;        
        i=1;
        while i<=n
            p_rand(i,1)=1/10*n/d*(threads-1)*f_avg_x+9/10*p_rand(i,1);
            i=i+1;
        end
    end
    Time1=R;
    fprintf("\t%f\t%f", Time1, threads/Time1);
    Throughput1(threads)=threads/Time1;
    Abort_probability1(threads)=1-1/V(1);
    
    %------------------------
    
    R=epsilon;
    while (abs(R_prev-R)>=epsilon)
        R_prev=R;
        l=zeros(n,1);
        f=zeros(n,1);
        V=zeros(n,1);
        i=1;
        while i<=n
            k=i:n; V(i)=(-1)^(n-(i-1))*(1/(prod(p_incr(k)-1)));
            i=i+1;
        end
        R=dot(V,T)+tc;
        i=1;
        while i<=n
            k=i+1;
            while k<=n
                l(i)=l(i)+V(k)*T(k);
                k=k+1;
            end
            l(i)=l(i)+tc;
            i=i+1;
        end
        i=1;
        while i<=n
            f(i)=l(i)/R;
            i=i+1;
        end
        i=1;
        while i<=n
            sum=0;
            x=1;
            while (x<=d)
                f_avg_x=0;
                k=1;
                while (k<=n)
                    f_avg_x=f_avg_x+access_probability(x,k,d,n)*f(k);
                    k=k+1;
                end
                sum=sum+access_probability(x,i,d,n)*f_avg_x;
                x=x+1;
            end
            p_incr(i)=1/10*(sum*(threads-1))+9/10*p_incr(i);
            %p_incr(i)=1/10*(1/s*(threads-1)*f(i))+9/10*p_incr(i);
            i=i+1;
        end
    end
    Time2=R;
    fprintf("\t%f", Time2, threads/Time2);
    Throughput2(threads)=threads/Time2;
    Abort_probability2(threads)=1-1/V(1);
    
    %------------------------
    
    threadsG2=threads*perc_threadsG2_run/100;
    threadsG1=threads-threadsG2;
    R_G1=epsilon;
    R_G2=epsilon;
    R_prev_g1=0;
    R_prev_g2=0;
    
    while (abs(R_prev_g1-R_G1)>=epsilon && abs(R_prev_g2-R_G2)>=epsilon)
        R_prev_g1=R_G1;
        R_prev_g2=R_G2;
        l_G1=zeros(n,1);
        f_G1=zeros(n,1);
        V_G1=zeros(n,1);
        l_G2=zeros(n,1);
        f_G2=zeros(n,1);
        V_G2=zeros(n,1);
        i=1;
        while i<=n
            k=i:n; V_G1(i)=(-1)^(n-(i-1))*(1/(prod(p_G1_r1(k)-1)));
            k=i:n; V_G2(i)=(-1)^(n-(i-1))*(1/(prod(p_G2_r1(k)-1)));
            i=i+1;
        end
        R_G1=dot(V_G1,T)+tc;
        R_G2=dot(V_G2,T)+tc;
        i=1;
        while i<=n
            k=i+1;
            while k<=n
                l_G1(i)=l_G1(i)+V_G1(k)*T(k);
                l_G2(i)=l_G2(i)+V_G2(k)*T(k);
                k=k+1;
            end
            l_G1(i)=l_G1(i)+tc;
            l_G2(i)=l_G2(i)+tc;
            i=i+1;
        end
        i=1;
        while i<=n
            f_G1(i)=l_G1(i)/R_G1;
            f_G2(i)=l_G2(i)/R_G2;
            i=i+1;
        end
        i=1;
        while i<=n
            x=1;
            sum_G1_G1=0;
            sum_G1_G2=0;
            sum_G2_G1=0;
            sum_G2_G2=0;
            while x<=d;
                f_avg_x_G1=0;
                f_avg_x_G2=0;
                k=1;
                while (k<=n)
                    ap1=access_probability(x,k,d,n);
                    f_avg_x_G1=f_avg_x_G1+ap1*f_G1(k);
                    ap2=1/d;
                    f_avg_x_G2=f_avg_x_G2+ap2*f_G2(k);
                    k=k+1;
                end
                ap=access_probability(x,i,d,n);
                sum_G1_G1=sum_G1_G1+ap*f_avg_x_G1;
                sum_G1_G2=sum_G1_G2+ap*f_avg_x_G2;
                
                ap=1/d;
                sum_G2_G1=sum_G2_G1+ap*f_avg_x_G1;
                sum_G2_G2=sum_G2_G2+ap*f_avg_x_G2;
                
                x=x+1;
            end
            p_G1_r1(i)=1/10*(sum_G1_G1*(threadsG1-1)+sum_G1_G2*(threadsG2))+9/10*p_G1_r1(i);
            p_G2_r1(i)=1/10*(sum_G2_G1*(threadsG1)+sum_G2_G2*(threadsG2-1))+9/10*p_G2_r1(i);
            i=i+1;
        end
    end
    	
    V1_G1=V_G1(1);
    V1_G2=V_G2(1);
    TimeG1=R_G1;
    TimeG2=R_G2;
    
    Time3=TimeG1*threadsG1/threads + TimeG2*threadsG2/threads;
    fprintf("\t%f\t%f", Time3, threads/Time3);
    Throughput3(threads)=threads/Time3;
    Abort_probability3(threads)=1-1/(V1_G1*threadsG1/threads + V1_G2*threadsG2/threads);    
    
    %------------------------
    
    threadsG2=threads*perc_threadsG2_run/100;
    threadsG1=threads-threadsG2;
    R_G1=epsilon;
    R_G2=epsilon;
    R_prev_g1=0;
    R_prev_g2=0;
    
    while (abs(R_prev_g1-R_G1)>=epsilon && abs(R_prev_g2-R_G2)>=epsilon)
        R_prev_g1=R_G1;
        R_prev_g2=R_G2;
        l_G1=zeros(n,1);
        f_G1=zeros(n,1);
        V_G1=zeros(n,1);
        l_G2=zeros(n,1);
        f_G2=zeros(n,1);
        V_G2=zeros(n,1);
        i=1;
        while i<=n
            k=i:n; V_G1(i)=(-1)^(n-(i-1))*(1/(prod(p_G1_r2(k)-1)));
            k=i:n; V_G2(i)=(-1)^(n-(i-1))*(1/(prod(p_G2_r2(k)-1)));
            i=i+1;
        end
        R_G1=dot(V_G1,T)+tc;
        R_G2=dot(V_G2,T)+tc;
        i=1;
        while i<=n
            k=i+1;
            while k<=n
                l_G1(i)=l_G1(i)+V_G1(k)*T(k);
                l_G2(i)=l_G2(i)+V_G2(k)*T(k);
                k=k+1;
            end
            l_G1(i)=l_G1(i)+tc;
            l_G2(i)=l_G2(i)+tc;
            i=i+1;
        end
        i=1;
        while i<=n
            f_G1(i)=l_G1(i)/R_G1;
            f_G2(i)=l_G2(i)/R_G2;
            i=i+1;
        end
        i=1;
        while i<=n
            x=1;
            sum_G1_G1=0;
            sum_G1_G2=0;
            sum_G2_G1=0;
            sum_G2_G2=0;
            while x<=d
                f_avg_x_G1=0;
                f_avg_x_G2=0;
                k=1;
                while (k<=n)
                    ap1=access_probability(x,k,d,n);
                    f_avg_x_G1=f_avg_x_G1+ap1*f_G1(k);
                    ap2=access_probability(d-x+1,k,d,n);
                    f_avg_x_G2=f_avg_x_G2+ap2*f_G2(k);
                    k=k+1;
                end
                ap=access_probability(x,i,d,n);
                sum_G1_G1=sum_G1_G1+ap*f_avg_x_G1;
                sum_G1_G2=sum_G1_G2+ap*f_avg_x_G2;
                
                ap=access_probability(d-x+1,i,d,n);
                sum_G2_G1=sum_G2_G1+ap*f_avg_x_G1;
                sum_G2_G2=sum_G2_G2+ap*f_avg_x_G2;
                
                x=x+1;
            end
            p_G1_r2(i)=1/10*(sum_G1_G1*(threadsG1-1)+sum_G1_G2*(threadsG2))+9/10*p_G1_r2(i);
            p_G2_r2(i)=1/10*(sum_G2_G1*(threadsG1)+sum_G2_G2*(threadsG2-1))+9/10*p_G2_r2(i);
            i=i+1;
        end
    end
    V1_G1=V_G1(1);
    V1_G2=V_G2(1);    
    TimeG1=R_G1;
    TimeG2=R_G2;
    
    Time4=TimeG1*threadsG1/threads + TimeG2*threadsG2/threads;
    fprintf("\t%f\t%f\n", Time4, threads/Time4);
    Throughput4(threads)=threads/Time4;
    threads=threads+1;
    
end

x = 1:length(Throughput1);   
figure(1);
plot(x, Throughput1, x, Throughput2, x, Throughput3,x, Throughput4)
title('Throughputs')
grid
x = 1:length(Abort_probability1);  
figure(2);
plot(x, Abort_probability1, x, Abort_probability2, x, Abort_probability3, x, Abort_probability4)
title('Abort probabilities') 
grid


function ap=access_probability(x,k,d,n)
if (x >= d/n * (k-1) && x < d/n * k)
    ap=1/(d/n);
else
    ap=0;
end
end
