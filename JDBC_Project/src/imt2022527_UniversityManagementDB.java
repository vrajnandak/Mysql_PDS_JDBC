import java.sql.*;
import java.util.Scanner;


public class imt2022527_UniversityManagementDB
{
    // Set JDBC driver name and database URL
   static final String JDBC_DRIVER = "com.mysql.cj.jdbc.Driver";
   static final String DB_URL = "jdbc:mysql://localhost:3306/UniversityManagement";

   // Database credentials
   static final String USER = "root";// add your user
   static final String PASSWORD = "Mysql@123";// add password

    public static void main(String[] args)
    {
        Connection conn = null;
        Statement stmt = null;

        //Connecting to Database.
        try
        {
            //Registering JDBC Driver.
            Class.forName(JDBC_DRIVER);

            //Opening a connection.
            System.out.println("Connecting to database...");
            conn=DriverManager.getConnection(DB_URL,USER,PASSWORD);
            conn.setAutoCommit(false);

            //Executing the query.
            stmt=conn.createStatement();

            // stmt.executeUpdate(create);
            Scanner scanner = new Scanner(System.in);
            boolean exit = false;

            while(exit==false)
            {   
                
                System.out.println("Please select an option:");
                System.out.println("1. View All Faculty Details (Result: List of all faculty details)");
                System.out.println("2. View All Course Details (Result: List of all course details)");
                System.out.println("3. View All Student Details (Result: List of all student details)");
                System.out.println("4. View All Enrollment Details (Result: List of all enrollment details)");
                System.out.println("5. View All Courses Taught By Faculty (Result: List of courses taught by faculty)");
                System.out.println("6. View All Department Details (Result: List of all department details)");
                System.out.println("7. View Faculty Details by Name (Result: Faculty details for a specific name)");
                System.out.println("8. View Faculty Details by ID (Result: Faculty details for a specific ID)");
                System.out.println("9. View Course Details by ID (Result: Course details for a specific ID)");
                System.out.println("10. View Course Details by Name (Result: Course details for a specific name)");
                System.out.println("11. View Student Details by ID (Result: Student details for a specific ID)");
                System.out.println("12. View Student Details by Name (Result: Student details for a specific name)");
                System.out.println("13. View Department Details by ID (Result: Department details for a specific ID)");
                System.out.println("14. View Department Details by Name (Result: Department details for a specific name)");
                System.out.println("15. View Courses Taught by Faculty ID (Result: Courses taught by a specific faculty)");
                System.out.println("16. View Students Enrolled in Courses by Student ID (Result: Students enrolled in courses for a specific ID");
                System.out.println("17. View Students Enrolled in Course by Course ID (Result: Students enrolled in a specific course)");
                System.out.println("18. View Head of Department by Department ID (Result: Head of department for a specific ID)");
                System.out.println("19. Add Faculty (Result: Add a new faculty member)");
                System.out.println("20. Delete Faculty (Result: Delete a faculty member)");
                System.out.println("21. Update Department of Faculty ID (Result: Update department of a faculty member)");


                int option = scanner.nextInt();
                scanner.nextLine(); // Consume newline

                switch(option)
                {
                    // Select All tuples in the relation.
                    case 1: ViewAllFaculty(stmt);   //executeQuery.
                        break;
                    case 2: ViewAllCourses(stmt);   //executeQuery.
                        break;
                    case 3: ViewAllStudent(stmt);  //executeQuery.
                        break;
                    case 4: ViewAllEnrollmentDetails(stmt); //executeQuery.
                        break;
                    case 5: ViewAllCoursesTaughtByFaculty(stmt);    //executeQuery.
                        break;
                    case 6: ViewAllDepartmentDetails(stmt);    //executeQuery.
                        break;

                    case 7: ViewFacultyByName(stmt,scanner);  //executeQuery.
                        break;
                    case 8: ViewFacultyByID(stmt,scanner);
                        break;
                    case 9: ViewCourseByID(stmt,scanner);
                        break;
                    case 10: ViewCourseByName(stmt, scanner);  //executeQuery.
                        break;
                    case 11: ViewStudentByID(stmt, scanner);  //executeQuery.
                        break;
                    case 12: ViewStudentByName(stmt, scanner);  //executeQuery.
                        break;
                    case 13: ViewDepartmentByID(stmt, scanner);  //executeQuery.
                        break;
                    case 14: ViewDepartmentByName(stmt, scanner);  //executeQuery.
                        break;
                    case 15: ViewCoursesTaughtByFacultyID(stmt, scanner);  //executeQuery.
                        break;
                    case 16: ViewStudentEnrolledInCoursesByID(stmt, scanner);  //executeQuery.
                        break;
                    case 17: ViewStudentsEnrolledInCourseByID(stmt, scanner);  //executeQuery.
                        break;
                    case 18: ViewHeadOfDepartmentByDeptID(stmt, scanner);  //executeQuery.
                        break;
                    case 19: AddFaculty(stmt, conn, scanner);  //executeUpdate.
                        break;
                    case 20: DeleteFaculty(stmt, conn, scanner);  //executeUpdate.
                        break;
                    case 21: UpdateDepartmentOfFacultyID(stmt,conn, scanner);  //executeUpdate.
                        break;
                    default:
                        exit=true;
                        break;
                    // case 13: SearchStudentsByCourses(stmt,scanner);
                }
            }
            stmt.close();
            conn.close();
            scanner.close();
        }
        catch(SQLException se)
        {
            //Handle errors for JDBC.
            se.printStackTrace();

            //If there is an error, then rollback the changes.
            try
            {
                if(conn!=null)
                {
                    conn.rollback();
                }
            }
            catch(SQLException se2)
            {
                System.out.println("Rollback failed...");
                se2.printStackTrace();
            }
        }
        catch(Exception e)
        {
            e.printStackTrace();
        }
        finally
        {
            try
            {
                if(stmt!=null)
                {
                    stmt.close();
                }
            }
            catch(SQLException se2)
            {}

            try
            {
                if(conn!=null)
                {
                    conn.close();
                }
            }
            catch(SQLException se3)
            {
                se3.printStackTrace();
            }
        }

        System.out.println("End of Code");
    }

    private static void ViewAllFaculty(Statement stmt) throws SQLException
    {
        String selectQuery="SELECT * FROM FACULTY";
        ResultSet rs = stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int FacultyID=rs.getInt("FacultyID");
            String FName=rs.getString("FName");
            String MName=rs.getString("MName");
            String LName=rs.getString("LName");
            String Email=rs.getString("Email");
            int DeptID=rs.getInt("DeptID");
            System.out.println("Faculty ID: " + FacultyID + ", Fname: " + FName + ", MName: " + MName + ", LName: " + LName + ", Email: " + Email + ", Department ID: " + DeptID);
        }
        rs.close();
    }

    private static void ViewAllCourses(Statement stmt) throws SQLException
    {
        String selectQuery="SELECT * FROM COURSE";
        ResultSet rs = stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int CourseID=rs.getInt("CourseID");
            String CourseName=rs.getString("CourseName");
            int CourseCredits=rs.getInt("CourseCredits");
            int DeptID=rs.getInt("DeptID");
            int FacultyID=rs.getInt("FacultyID");
            System.out.println("Course ID: " + CourseID + ", Course Name: " + CourseName + ", Credits: " + CourseCredits + "Department ID: " + DeptID + ", Faculty ID: " + FacultyID);
        }
        rs.close();
    }

    private static void ViewAllStudent(Statement stmt) throws SQLException
    {
        String selectQuery="SELECT * FROM STUDENT";
        ResultSet rs = stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int StudentID=rs.getInt("StudentID");
            String FName=rs.getString("FName");
            String LName=rs.getString("LName");
            String Email=rs.getString("Email");
            String DateOfBirth=rs.getString("DateOfBirth");
            String HomeAddr=rs.getString("HomeAddress");
            int ParentPhone = rs.getInt("ParentPhone");
            System.out.println("Student ID: " + StudentID + ", Fname: " + FName + ", LName: " + LName + ", Email: " + Email + ", Date of Birth: " + DateOfBirth + ", Address: " + HomeAddr + ", Parent Phone number: " + ParentPhone);
        }
        rs.close();
    }

    private static void ViewAllEnrollmentDetails(Statement stmt) throws SQLException
    {
        String selectQuery="SELECT * FROM ENROLLMENT";
        ResultSet rs = stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int EnrollmentID=rs.getInt("EnrollmentID");
            int StudentID=rs.getInt("StudentID");
            int CourseID=rs.getInt("CourseID");
            String EnrollmentDate=rs.getString("EnrollmentDate");
            System.out.println("Enrollment ID: " + EnrollmentID + ", Student ID: " + StudentID + ", Course ID: " + CourseID + ", Enrollment Date: " + EnrollmentDate);
        }
        rs.close();
    }

    private static void ViewAllCoursesTaughtByFaculty(Statement stmt) throws SQLException
    {
        String selectQuery="SELECT * FROM TEACHES";
        ResultSet rs = stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int FacultyID=rs.getInt("FacultyID");
            int CourseID=rs.getInt("CourseID");
            System.out.println("Faculty ID: " + FacultyID + "Course ID: " + CourseID);
        }
        rs.close();
    }

    private static void ViewAllDepartmentDetails(Statement stmt) throws SQLException
    {
        String selectQuery="SELECT * FROM DEPARTMENT";
        ResultSet rs = stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int DepartmentID=rs.getInt("DepartmentID");
            String DepartmentName=rs.getString("DepartmentName");
            int HeadOfDepartmentID=rs.getInt("HeadOfDepartmentID");
            String ContactInfo=rs.getString("ContactInformation");
            System.out.println("Department ID: " + DepartmentID + ", Department Name: " + DepartmentName + ", Head of Department ID: " + HeadOfDepartmentID + ", Contact Information: " + ContactInfo);
        }
        rs.close();
    }

    private static void ViewFacultyByID(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter The Faculty ID to search for in FACULTY");
        int FID=sc.nextInt();
        sc.nextLine();

        String selectQuery="SELECT * FROM FACULTY WHERE FacultyID = " + FID;
        ResultSet rs = stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int FacultyID=rs.getInt("FacultyID");
            String FName=rs.getString("FName");
            String MName=rs.getString("MName");
            String LName=rs.getString("LName");
            String Email=rs.getString("Email");
            int DeptID=rs.getInt("DeptID");
            System.out.println("Faculty ID: " + FacultyID + ", Fname: " + FName + ", MName: " + MName + ", LName: " + LName + ", Email: " + Email + ", Department ID: " + DeptID);
        }
        rs.close();
    }

    private static void ViewFacultyByName(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter the First name of the faculty to search for");
        String fname=sc.nextLine();

        String selectQuery="SELECT * FROM FACULTY WHERE FName = '" + fname + "'";
        ResultSet rs=stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int FacultyID=rs.getInt("FacultyID");
            String FName=rs.getString("FName");
            String MName=rs.getString("MName");
            String LName=rs.getString("LName");
            String Email=rs.getString("Email");
            int DeptID=rs.getInt("DeptID");
            System.out.println("Faculty ID: " + FacultyID + ", Fname: " + FName + ", MName: " + MName + ", LName: " + LName + ", Email: " + Email + ", Department ID: " + DeptID);
        }
        rs.close();
    }

    private static void ViewCourseByID(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter Course ID to search for");
        int CID=sc.nextInt();
        sc.nextLine();

        String selectQuery="SELECT * FROM COURSE WHERE CourseID=" + CID;
        ResultSet rs=stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int CourseID=rs.getInt("CourseID");
            String CourseName=rs.getString("CourseName");
            int CourseCredits=rs.getInt("CourseCredits");
            int DeptID=rs.getInt("DeptID");
            int FacultyID=rs.getInt("FacultyID");
            System.out.println("Course ID: " + CourseID + ", Course Name: " + CourseName + ", Credits: " + CourseCredits + "Department ID: " + DeptID + ", Faculty ID: " + FacultyID);
        }
        rs.close();
    }

    private static void ViewCourseByName(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter Course name");
        String cname=sc.nextLine();

        String selectQuery="SELECT * FROM COURSE WHERE CourseName LIKE '%" + cname + "%'";
        ResultSet rs=stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int CourseID=rs.getInt("CourseID");
            String CourseName=rs.getString("CourseName");
            int CourseCredits=rs.getInt("CourseCredits");
            int DeptID=rs.getInt("DeptID");
            int FacultyID=rs.getInt("FacultyID");
            System.out.println("Course ID: " + CourseID + ", Course Name: " + CourseName + ", Credits: " + CourseCredits + "Department ID: " + DeptID + ", Faculty ID: " + FacultyID);
        }
        rs.close();
    }

    private static void ViewStudentByID(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter Student ID to search for");
        int SID=sc.nextInt();
        sc.nextLine();

        String selectQuery="SELECT * FROM STUDENT WHERE StudentID=" + SID;
        ResultSet rs=stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int StudentID=rs.getInt("StudentID");
            String FName=rs.getString("FName");
            String LName=rs.getString("LName");
            String Email=rs.getString("Email");
            String DateOfBirth=rs.getString("DateOfBirth");
            String HomeAddr=rs.getString("HomeAddress");
            int ParentPhone = rs.getInt("ParentPhone");
            System.out.println("Student ID: " + StudentID + ", Fname: " + FName + ", LName: " + LName + ", Email: " + Email + ", Date of Birth: " + DateOfBirth + ", Address: " + HomeAddr + ", Parent Phone number: " + ParentPhone);
        }
        rs.close();
    }

    private static void ViewStudentByName(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter Student Name to search for");
        String name=sc.nextLine();

        String selectQuery="SELECT * FROM STUDENT WHERE FName='" + name + "'";
        ResultSet rs=stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int StudentID=rs.getInt("StudentID");
            String FName=rs.getString("FName");
            String LName=rs.getString("LName");
            String Email=rs.getString("Email");
            String DateOfBirth=rs.getString("DateOfBirth");
            String HomeAddr=rs.getString("HomeAddress");
            int ParentPhone = rs.getInt("ParentPhone");
            System.out.println("Student ID: " + StudentID + ", Fname: " + FName + ", LName: " + LName + ", Email: " + Email + ", Date of Birth: " + DateOfBirth + ", Address: " + HomeAddr + ", Parent Phone number: " + ParentPhone);
        }
        rs.close();
    }

    private static void ViewDepartmentByID(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter Department ID to search for");
        int DeptID=sc.nextInt();
        sc.nextLine();

        String selectQuery="SELECT * FROM DEPARTMENT WHERE DepartmentID=" + DeptID;
        ResultSet rs=stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int DepartmentID=rs.getInt("DepartmentID");
            String DepartmentName=rs.getString("DepartmentName");
            int HeadOfDepartmentID=rs.getInt("HeadOfDepartmentID");
            String ContactInfo=rs.getString("ContactInformation");
            System.out.println("Department ID: " + DepartmentID + ", Department Name: " + DepartmentName + ", Head of Department ID: " + HeadOfDepartmentID + ", Contact Information: " + ContactInfo);
        }
        rs.close();
    }

    private static void ViewDepartmentByName(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter Department Name to search for");
        String Dname=sc.nextLine();

        String selectQuery="SELECT * FROM DEPARTMENT WHERE DepartmentName='" + Dname + "'";
        ResultSet rs=stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int DepartmentID=rs.getInt("DepartmentID");
            String DepartmentName=rs.getString("DepartmentName");
            int HeadOfDepartmentID=rs.getInt("HeadOfDepartmentID");
            String ContactInfo=rs.getString("ContactInformation");
            System.out.println("Department ID: " + DepartmentID + ", Department Name: " + DepartmentName + ", Head of Department ID: " + HeadOfDepartmentID + ", Contact Information: " + ContactInfo);
        }
        rs.close();
    }

    //Shows only those tuples in TEACHES that have the given Faculty ID.
    private static void ViewCoursesTaughtByFacultyID(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter Faculty ID to search for courses taught by the faculty");
        int FID=sc.nextInt();
        sc.nextLine();

        String selectQuery="SELECT FACULTY.FacultyID, FName, CourseID, CourseName FROM TEACHES,FACULTY,COURSE WHERE TEACHES.FacultyID=FACULTY.FacultyID AND TEACHES.CourseID=COURSE.CourseID AND FACULTY.FacultyID="+ FID;
        ResultSet rs=stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            int fid=rs.getInt("FacultyID");
            String Fname=rs.getString("FName");
            int cid=rs.getInt("CourseID");
            String cname=rs.getString("CourseName");
            System.out.println("Faculty " + Fname + " with ID: " + fid + ", Teaches Course " + cname + " " + cid );
        }
        rs.close();
    }

    private static void ViewStudentEnrolledInCoursesByID(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter Student ID to search for Courses the student is enrolled in");
        int sid=sc.nextInt();
        sc.nextLine();

        String selectQuery="SELECT ENROLLMENT.StudentID, STUDENT.FName, ENROLLMENT.CourseID, COURSE.CourseName FROM ENROLLMENT,STUDENT,COURSE WHERE ENROLLMENT.StudentID=STUDENT.StudentID AND ENROLLMENT.CourseID=COURSE.CourseID AND STUDENT.StudentID=" + sid;
        ResultSet rs=stmt.executeQuery(selectQuery);

        System.out.println("Student " + sid + " is enrolled in the following courses:");

        int counter=0;
        while(rs.next())
        {
            int result_sid=rs.getInt("ENROLLMENT.StudentID");
            String sname=rs.getString("STUDENT.FName");
            int cid=rs.getInt("ENROLLMENT.CourseID");
            String cname=rs.getString("COURSE.CourseName");
            System.out.println(counter + ". " + cname + " " + cid);
        }
        rs.close();
    }

    private static void ViewStudentsEnrolledInCourseByID(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter Course ID to search for Students enrolled in the Course");
        int cid=sc.nextInt();
        sc.nextLine();

        String selectQuery="SELECT ENROLLMENT.StudentID, STUDENT.FName, ENROLLMENT.CourseID, COURSE.CourseName FROM ENROLLMENT,STUDENT,COURSE WHERE ENROLLMENT.StudentID=STUDENT.StudentID AND ENROLLMENT.CourseID=COURSE.CourseID AND COURSE.CourseID=" + cid;
        ResultSet rs=stmt.executeQuery(selectQuery);

        System.out.println("The following students are enrolled in Course: " + cid);
        
        int counter=0;
        while(rs.next())
        {
            int sid=rs.getInt("ENROLLMENT.StudentID");
            String sname=rs.getString("STUDENT.FName");
            int result_cid=rs.getInt("ENROLLMENT.CourseID");
            String cname=rs.getString("COURSE.CourseName");
            System.out.println(counter + ". " + sname + " " + sid);
        }
        rs.close();
    }

    private static void ViewHeadOfDepartmentByDeptID(Statement stmt, Scanner sc) throws SQLException
    {
        System.out.println("Enter the Department ID whose Head is to be found");
        int deptID=sc.nextInt();
        sc.nextLine();

        String selectQuery="SELECT DepartmentName,FacultyID,FName FROM FACULTY, DEPARTMENT WHERE HeadOfDepartmentID=FacultyID AND DepartmentID="+ deptID;
        ResultSet rs=stmt.executeQuery(selectQuery);
        while(rs.next())
        {
            String name=rs.getString("DepartmentName");
            int fid=rs.getInt("FacultyID");
            String fname=rs.getString("FName");
            System.out.println("Head of " + name + " is : " + fname + " " + fid);
        }
        rs.close();
    }

    private static void AddFaculty(Statement stmt, Connection conn, Scanner sc) throws SQLException
    {
        System.out.println("Enter the Faculty ID of the new Faculty");
        int fid=sc.nextInt();
        
        sc.nextLine();

        System.out.println("Enter FName of Faculty to be added");
        String fname=sc.nextLine();

        System.out.println("Enter MName of Faculty to be added");
        String mname=sc.nextLine();

        System.out.println("Enter LName of Faculty to be added");
        String lname=sc.nextLine();
        
        System.out.println("Enter the Email of Faculty");
        String email=sc.nextLine();

        //We insert the tuple with the Foreign key as null for now.
        String INSERT_FACULTY="INSERT INTO FACULTY VALUES(" + fid + ",'"+fname+"','"+mname+"','"+lname+"','"+email+"',"+"NULL)";
        stmt.executeUpdate(INSERT_FACULTY);

        System.out.println("Enter the Department ID of the Faculty");
        int deptID=sc.nextInt();

        sc.nextLine();

        try
        {
            //Updating the Department ID of the tuple inserted above with the DeptID attribute value having NULL.
            String updateQuery="UPDATE FACULTY SET DeptID="+deptID+" WHERE FacultyID="+fid;
            int rowsUpdated=stmt.executeUpdate(updateQuery);

            if(rowsUpdated>0)
            {
                conn.commit();
                System.out.println("Committed changes");
            }
            else
            {
                System.out.println("Facult ID " +fid + " does not exist");
            }
        }
        catch(SQLException e)
        {
            try
            {
               if(conn!=null)
               {
                    System.out.println("Rolling back...");
                    conn.rollback();
                    System.out.println("Rolling back Successfull...");
               }
            }
            catch(SQLException se3)
            {
               se3.printStackTrace();
            }
        }
    }

    private static void DeleteFaculty(Statement stmt, Connection conn, Scanner sc) throws SQLException
    {
        System.out.println("Enter the Faculty ID to be deleted");
        int fid=sc.nextInt();
        sc.nextLine();

        try
        {
            String updateQuery="DELETE FROM FACULTY WHERE FacultyID="+fid;
            int rowsDeleted=stmt.executeUpdate(updateQuery);
            if(rowsDeleted>0)
            {
                System.out.println("Faculty deleted successfully...");
                conn.commit();
                System.out.println("Committed Changes.");
            }
            else
            {
                System.out.println("Faculty not found");
            }
        }
        catch(SQLException e)
        {
            e.printStackTrace();
        }
    }

    private static void UpdateDepartmentOfFacultyID(Statement stmt,Connection conn, Scanner sc) throws SQLException
    {
        System.out.println("Enter the Faculty ID whose Department is to be changed.");
        int fid=sc.nextInt();

        System.out.println("Enter the new Department ID of the Faculty");
        int newDeptID=sc.nextInt();
        sc.nextLine();

        try
        {
            String updateQuery="UPDATE FACULTY SET DeptID=" + newDeptID + " WHERE FacultyID=" + fid;
            int rowsUpdated=stmt.executeUpdate(updateQuery);
            if(rowsUpdated>0)
            {
                System.out.println("Updated Department Successfully");
                conn.commit();
                System.out.println("Committted changes");
            }
            else
            {
                System.out.println("Faculty Does not exist");
            }
        }
        catch(SQLException e)
        {
            try
            {
               if(conn!=null)
               {
                    System.out.println("Rolling back...");
                    conn.rollback();
                    System.out.println("Rolling back Successfull...");
               }
            }
            catch(SQLException se3)
            {
               se3.printStackTrace();
            }
        }

    }
}
