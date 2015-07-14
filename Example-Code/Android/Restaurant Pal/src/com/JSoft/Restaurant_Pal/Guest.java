package com.JSoft.Restaurant_Pal;

/**
 * Created by Jared Fowler on 4/8/2014.
 *
 * Notes:
 *
 * The information is limited. I don't expect users
 * to send much personal information during a survey!
 *
 */

public class Guest {

    //********************************************************************
    //CONSTANTS***********************************************************
    //********************************************************************
    public static final String DEFAULT_EMAIL = "Unknown_Email";
    public static final int DEFAULT_AGE = 0;
    public static final String DEFAULT_GENDER = "Unknown_Gender";

    //********************************************************************
    //VARIABLES***********************************************************
    //********************************************************************
    private String emailAddress = null;
    private int age;
    private String gender = null;

    //********************************************************************
    //METHODS*************************************************************
    //********************************************************************
    //Constructors
    public Guest(){
       this(DEFAULT_EMAIL, DEFAULT_AGE, DEFAULT_GENDER);
    }
    //---------------------------------------------------------------------
    public Guest(String s){
        this(s, DEFAULT_AGE, DEFAULT_GENDER);
    }
    //---------------------------------------------------------------------
    public Guest(String s, int i){
        this(s, i, DEFAULT_GENDER);
    }
    //---------------------------------------------------------------------
    public Guest(String s1, int i, String s2){
        this.setEmailAddress(s1);
        this.setAge(i);
        this.setGender(s2);
    }
    //---------------------------------------------------------------------
    public Guest(Guest g){
        this(g.getEmailAddress(), g.getAge(), g.getGender());
    }
    //---------------------------------------------------------------------
    public String getEmailAddress(){return this.emailAddress;}
    //---------------------------------------------------------------------
    public void setEmailAddress(String s){
        this.emailAddress = (s == null || s.isEmpty()) ? DEFAULT_EMAIL : s;
    }
    //---------------------------------------------------------------------
    public int getAge(){return this.age;}
    //---------------------------------------------------------------------
    public void setAge(int i){
        this.age = (i < 0 || i > 150) ? DEFAULT_AGE : i;
    }
    //---------------------------------------------------------------------
    public String getGender(){return this.gender;}
    //---------------------------------------------------------------------
    public void setGender(String s){
        this.gender = (s == null || s.isEmpty()) ? DEFAULT_GENDER : s;
    }
    //---------------------------------------------------------------------
    public String toString(){
        return new String(this.getEmailAddress() + " " + this.getAge() + " " + this.getGender());
    }

}