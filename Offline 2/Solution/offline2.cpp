#include<bits/stdc++.h>

using namespace std;

#define PI acos(-1.0)
FILE *out;
FILE *in;
double eps=1e-9;
struct Vector
{
    vector<double> point;

    Vector()
    {
    }
    Vector(double x,double y,double z,double w=1)
    {
        point.push_back(x);
        point.push_back(y);
        point.push_back(z);
        point.push_back(w);
    }
    Vector (vector<double>v)
    {
        point=v;

    }

    void Set(double x,double y,double z,double w=1)
    {
        point.push_back(x);
        point.push_back(y);
        point.push_back(z);
        point.push_back(w);
    }

    void scalling()
    {
        for(int i=0; i<4; i++)
        {
            point[i]/=point[3];
        }
    }
    void normalized()
    {
        double val=0;
        for(int i=0; i<3; i++)
        {
            val+=(point[i]*point[i]);
        }
        val=sqrt(val);
        for(int i=0; i<3; i++)
        {
            point[i]/=val;
        }

    }
    double dot(Vector v)
    {
        double ans=0.0;
        for(int i=0; i<3; i++)
        {
            ans+=(point[i]*v.point[i]);
        }

        return ans;

    }
    Vector cross(Vector v)
    {
        Vector temp;
        temp.point.push_back(point[1]*v.point[2]-point[2]*v.point[1]);
        temp.point.push_back(-point[0]*v.point[2]+point[2]*v.point[0]);
        temp.point.push_back(point[0]*v.point[1]-point[1]*v.point[0]);
        temp.point.push_back(1);
        return temp;

    }
    void scalermul(double val)
    {
        for(int i=0; i<3; i++)
        {
            point[i]*=val;
        }
        /// normalized();
    }
    Vector Add(Vector v)
    {
        Vector temp;
        for(int i=0; i<3; i++)
        {
            temp.point.push_back(point[i]+v.point[i]);
        }
        return temp;

    }
    Vector Sub(Vector v)
    {
        Vector temp;
        for(int i=0; i<3; i++)
        {
            temp.point.push_back(point[i]-v.point[i]);
        }
        return temp;

    }
    friend ostream& operator<< (ostream &ostrm, Vector const& v);


};

Vector eye,look,up;
double fovY,aspectRatio,near,far;

ostream& operator<< (ostream &ostrm, Vector const& v)
{
    for(int i=0; i<3; i++)
    {

        fprintf(out,"%.7f ",v.point[i]);
        //out<<v.point[i]<<" ";
    }
    fprintf(out,"\n");
    return ostrm;
}




struct matrix
{


    vector<vector<double> > mat;
    matrix()
    {
    }
    matrix(vector<vector<double> >v)
    {
        mat=v;
    }


    matrix operator *( matrix &m2)
    {

        vector<vector<double> > v;
        for(int i=0; i<4; i++)
        {
            vector<double> x;
            for(int j=0; j<4; j++)
            {
                double mul=0;
                for(int k=0; k<4; k++)
                {
                    mul+=(mat[i][k]*m2.mat[k][j]);


                }
                x.push_back(mul);
            }
            v.push_back(x);

        }


        matrix temp(v);
        return temp;
    }
    friend ostream& operator<< (ostream &out, matrix const& m);


};

ostream& operator<< (ostream &out, matrix const& m)
{
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            if(fabs(m.mat[i][j]-0.0)<eps)
            {
                out<<0<<" ";
            }
            else
                out<<m.mat[i][j]<<" ";
        }
        out<<endl;
    }

    return out;
}
matrix Identity()
{
    vector<vector<double> > v;
    for(int i=0; i<4; i++)
    {
        vector<double > x;
        for(int j=0; j<4; j++)
        {
            if(i==j)
            {
                x.push_back(1);
            }
            else
                x.push_back(0);
        }
        v.push_back(x);
    }

    matrix temp(v);
    return temp;

}



matrix Translate(double tx,double ty,double tz)
{
    vector<vector<double> > v;
    for(int i=0; i<4; i++)
    {
        vector<double > x;
        for(int j=0; j<4; j++)
        {
            if(i==j)
            {
                x.push_back(1);
            }
            else if(j==3)
            {
                if(i==0)
                {
                    x.push_back(tx);
                }
                else if(i==1)
                {
                    x.push_back(ty);
                }
                else
                    x.push_back(tz);
            }
            else
                x.push_back(0);
        }
        v.push_back(x);
    }

    matrix temp(v);
    return temp;

}
matrix Scalling(double sx,double sy,double sz)
{
    vector<vector<double> > v;
    for(int i=0; i<4; i++)
    {
        vector<double > x;
        for(int j=0; j<4; j++)
        {
            if(i==j and i!=3)
            {
                if(i==0)
                {
                    x.push_back(sx);
                }
                else if(i==1)
                {
                    x.push_back(sy);
                }
                else
                    x.push_back(sz);
            }
            else
            {

                x.push_back(0);

                if(i==3 and j==3)
                {
                    x[3]=1;
                }
            }

        }
        v.push_back(x);
    }

    matrix temp(v);
    return temp;

}
matrix Rotation(Vector c1,Vector c2,Vector c3)
{
    vector<vector<double> >v;

    for(int i=0; i<4; i++)
    {
        vector<double > x;
        if(i<3)
        {
            x.push_back(c1.point[i]);
            x.push_back(c2.point[i]);
            x.push_back(c3.point[i]);
            x.push_back(0);

        }
        else
        {
            for(int j=0; j<3; j++)
            {
                x.push_back(0);
            }
            x.push_back(1);

        }
        v.push_back(x);
    }
    matrix temp(v);
    return temp;


}
Vector Rodrigues(Vector x,Vector a,double ang)
{

    Vector v1=a.cross(x);

    v1.scalermul(sin(ang*PI/180.0));

    double val=a.dot(x);


    Vector v2=a;
    v2.scalermul(val);
    v2.scalermul(1.0-cos((ang/180.0)*PI));

    Vector v3=x;
    v3.scalermul(cos((ang/180.0)*PI));
    ///cout<<v1<<v2<<cos(PI/2);
    Vector ans=(v3.Add(v2));
    ///ans.scalling();
    ans=ans.Add(v1);
    /// ans.scalling();
    /// cout<<ans<<endl;
    return ans;



}

Vector transformation(Vector v,matrix m)
{
    vector<double>ans;

    for(int i=0; i<4; i++)
    {
        double x=0;
        for(int j=0; j<4; j++)
        {
            x+=(v.point[j]*m.mat[i][j]);
        }
        ans.push_back(x);
        ///fprintf(out,"%.6f ",x);
    }
    ///fprintf(out,"\n");

    Vector temp(ans);

    return temp;



}
int main()
{
    for(int cs=1; cs<=3; cs++)
    {
        in=fopen("scene.txt","r");
        if(cs==1)
            out=fopen("stage1.txt","w");
        else if(cs==2)
        {
            out=fopen("stage2.txt","w");
        }
        else
        {
            out=fopen("stage3.txt","w");
        }
        double ex,ey,ez;
        fscanf(in,"%lf%lf%lf",&ex,&ey,&ez);
        // cin>>ex>>ey>>ez;

        eye.Set(ex,ey,ez);
        ///  cout<<eye;

        double lx,ly,lz;

        fscanf(in,"%lf%lf%lf",&lx,&ly,&lz);

        look.Set(lx,ly,lz);
        ///cout<<look;

        double ux,uy,uz;
        fscanf(in,"%lf%lf%lf",&ux,&uy,&uz);

        up.Set(ux,uy,uz);
        /// cout<<up;
        fscanf(in,"%lf%lf%lf%lf",&fovY,&aspectRatio,&near,&far);
        ///cin>>fovY>>aspectRatio>>near>>far;
        ///cout<<fovY<<" "<<aspectRatio<<" "<<near<<" "<<far<<endl;/



        stack<matrix>st;
        if(cs==1)
        {
            st.push(Identity());
        }
        else if(cs==2)
        {

            Vector l=look;
            l=l.Sub(eye);
            l.normalized();


            Vector r=l.cross(up);
            r.normalized();

            Vector u=r.cross(l);
            matrix T=Translate(-eye.point[0],-eye.point[1],-eye.point[2]);

            matrix R;
            for(int i=0; i<4; i++)
            {
                vector<double>x;
                for(int j=0; j<3; j++)
                {
                    if(i==0)
                    {
                        x.push_back(r.point[j]);
                    }
                    else if(i==1)
                    {
                        x.push_back(u.point[j]);
                    }
                    else if(i==2)
                    {
                        x.push_back(-l.point[j]);
                    }
                    else
                        x.push_back(0);
                }
                if(i!=3)
                    x.push_back(0);
                else
                    x.push_back(1);
                R.mat.push_back(x);
            }


            matrix V=R*T;
            st.push(V);

        }
        else{


            Vector l=look;
            l=l.Sub(eye);
            l.normalized();


            Vector r=l.cross(up);
            r.normalized();

            Vector u=r.cross(l);
            matrix T=Translate(-eye.point[0],-eye.point[1],-eye.point[2]);

            matrix R;
            for(int i=0; i<4; i++)
            {
                vector<double>x;
                for(int j=0; j<3; j++)
                {
                    if(i==0)
                    {
                        x.push_back(r.point[j]);
                    }
                    else if(i==1)
                    {
                        x.push_back(u.point[j]);
                    }
                    else if(i==2)
                    {
                        x.push_back(-l.point[j]);
                    }
                    else
                        x.push_back(0);
                }
                if(i!=3)
                    x.push_back(0);
                else
                    x.push_back(1);
                R.mat.push_back(x);
            }


            matrix V=R*T;
            ///st.push(V);

            double fovX=fovY*aspectRatio;
            double t=near*tan((fovY/2.0)*PI/180.0);
            double r1=near*tan((fovX/2.0)*PI/180.0);
            matrix P;

            for(int i=0;i<4;i++){
                vector<double >x;
                for(int j=0;j<4;j++){
                    if(i==0 and j==0){
                        x.push_back(near/r1);
                    }
                    else if(i==1 and j==1){
                        x.push_back(near/t);
                    }
                    else if(i==2 and j==2){
                        x.push_back(-(far+near)/(far-near));
                    }
                    else if(i==2 and j==3){
                        x.push_back(-(2*far*near)/(far-near));
                    }
                    else if(i==3 and j==2){
                        x.push_back(-1.0);
                    }
                    else x.push_back(0.0);
                }
                P.mat.push_back(x);
            }
            ///cout<<P<<endl;
            ///cout<<V<<endl;
           /// cout<<P*V<<endl;

            st.push(P*V);
        }


        vector<stack<matrix> >vs;
        vs.push_back(st);

        char s[100];
        while(1)
        {
            fscanf(in,"%s",s);
            /// printf("%s",s);
            if(strncmp(s,"end",3)==0)
            {
                break;
            }
            else if(strncmp(s,"triangle",8)==0)
            {
                Vector c1,c2,c3;
                double x,y,z;
                fscanf(in,"%lf%lf%lf",&x,&y,&z);
                c1.Set(x,y,z);
                c1=transformation(c1,vs[vs.size()-1].top());
                c1.scalling();
                fscanf(in,"%lf%lf%lf",&x,&y,&z);
                c2.Set(x,y,z);
                c2=transformation(c2,vs[vs.size()-1].top());
                c2.scalling();
                fscanf(in,"%lf%lf%lf",&x,&y,&z);
                c3.Set(x,y,z);
                c3=transformation(c3,vs[vs.size()-1].top());
                c3.scalling();

                cout<<c1<<c2<<c3;
                fprintf(out,"\n");


            }
            else if(strncmp(s,"push",4)==0)
            {

                stack<matrix>nst;
                nst=vs.back();
                vs.push_back(nst);
            }
            else if(strncmp(s,"pop",4)==0)
            {
                vs.pop_back();
            }
            else if(strncmp(s,"scale",5)==0)
            {
                double sx,sy,sz;
                fscanf(in,"%lf%lf%lf",&sx,&sy,&sz);
                ///cout<<sx<<sy<<sz<<endl;
                matrix m=Scalling(sx,sy,sz);
                ///cout<<m<<endl;
                vs.back().push(vs.back().top()*m);
            }
            else if(strncmp(s,"translate",4)==0)
            {
                double tx,ty,tz;
                fscanf(in,"%lf%lf%lf",&tx,&ty,&tz);
                ///cout<<sx<<sy<<sz<<endl;
                matrix m=Translate(tx,ty,tz);
                ///cout<<m<<endl;
                vs.back().push(vs.back().top()*m);
            }
            else if(strncmp(s,"rotate",6)==0)
            {
                double ang;
                fscanf(in,"%lf",&ang);
                double ax,ay,az;
                fscanf(in,"%lf%lf%lf",&ax,&ay,&az);
                Vector a(ax,ay,az);
                a.normalized();
                Vector i(1,0,0);
                Vector j(0,1,0);
                Vector k(0,0,1);
                Vector c1=Rodrigues(i,a,ang);
                Vector c2=Rodrigues(j,a,ang);
                Vector c3=Rodrigues(k,a,ang);
                matrix m=Rotation(c1,c2,c3);
                ///cout<<c1<<c2<<c3<<endl<<m<<endl;
                vs.back().push(vs.back().top()*m);
                // cout<<vs.back().top();

            }
        }

        fclose(out);
        fclose(in);
    }


}
