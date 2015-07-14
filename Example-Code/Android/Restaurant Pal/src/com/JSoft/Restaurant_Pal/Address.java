package com.JSoft.Restaurant_Pal;

/**
 * Created by Jared Fowler on 4/8/2014.
 */

public class Address {

    //********************************************************************
    //CONSTANTS***********************************************************
    //********************************************************************
    public static final String DEFAULT_STREET = "Unknown_Street";
    public static final String DEFAULT_CITY = "Unknown_City";
    public static final String DEFAULT_STATE = "Unknown_State";
    public static final String DEFAULT_POSTALCODE = "00000";
    public static final String DEFAULT_COUNTRY = "Unknown_Country";

    //********************************************************************
    //VARIABLES***********************************************************
    //********************************************************************
    private String streetAddress = null;
    private String city = null;
    private String state = null;
    private String postalCode;
    private String country = null;

    //********************************************************************
    //METHODS*************************************************************
    //********************************************************************
    //Constructors
    Address(){
        this(DEFAULT_STREET, DEFAULT_CITY, DEFAULT_STATE, DEFAULT_POSTALCODE, DEFAULT_COUNTRY);
    }
    //---------------------------------------------------------------------
    Address(String s1, String s2, String s3, String s4, String s5){
        this.setStreetAddress(s1);
        this.setCity(s2);
        this.setState(s3);
        this.setPostalCode(s4);
        this.setCountry(s5);
    }
    //---------------------------------------------------------------------
    Address(Address a){
        this(a.getStreetAddress(), a.getCity(), a.getState(), a.getPostalCode(), a.getCountry());
    }
    //---------------------------------------------------------------------
    public String getStreetAddress(){return this.streetAddress;}
    //---------------------------------------------------------------------
    public void setStreetAddress(String s){
        this.streetAddress = (s == null || s.isEmpty()) ? DEFAULT_STREET : s;
    }
    //---------------------------------------------------------------------
    public String getCity(){return this.city;}
    //---------------------------------------------------------------------
    public void setCity(String s){
        this.city = (s == null || s.isEmpty()) ? DEFAULT_CITY : s;
    }
    //---------------------------------------------------------------------
    public String getState(){return this.state;}
    //---------------------------------------------------------------------
    public void setState(String s){
        this.state = (s == null || s.isEmpty()) ? DEFAULT_STATE : s;
    }
    //---------------------------------------------------------------------
    public String getPostalCode(){return this.postalCode;}
    //---------------------------------------------------------------------
    public void setPostalCode(String s){
        this.state = (s == null || s.isEmpty()) ? DEFAULT_POSTALCODE : s;
    }
    //---------------------------------------------------------------------
    public String getCountry(){return this.country;}
    //---------------------------------------------------------------------
    public void setCountry(String s){
        this.country = (s == null || s.isEmpty()) ? DEFAULT_COUNTRY : s;
    }
    //---------------------------------------------------------------------
    public String toString(){
        return new String(streetAddress + " " + city + " " + state + " " + postalCode
                          + " " + country);
    }

}