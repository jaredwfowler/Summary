package com.JSoft.Restaurant_Pal;

import com.JSoft.Restaurant_Pal.Address;

/**
 * Created by Jared Fowler on 4/8/2014.
 *
 */

public class Building {

    //********************************************************************
    //CONSTANTS***********************************************************
    //********************************************************************
    public static final String DEFAULT_NAME = "Unknown_Building";

    //********************************************************************
    //VARIABLES***********************************************************
    //********************************************************************
    private String name = null;
    private Address address = null;

    //********************************************************************
    //METHODS*************************************************************
    //********************************************************************
    //Constructors
    public Building(){
        this(DEFAULT_NAME, null);
    }
    //---------------------------------------------------------------------
    public Building(String s){
        this(s,null);
    }
    //---------------------------------------------------------------------
    public Building(Address a){
        this(DEFAULT_NAME, a);
    }
    //---------------------------------------------------------------------
    public Building(String s, Address a){
        this.setAddress(a);
        this.setName(s);
    }
    //---------------------------------------------------------------------
    //Copy Constructor..of a sense
    public Building(Building b){
        this(b.getName(), b.getAddress());
    }
    //---------------------------------------------------------------------
    public String getName(){return this.name;}
    //---------------------------------------------------------------------
    public void setName(String s){
        this.name = (s == null || s.isEmpty()) ? DEFAULT_NAME : s;
    }
    //---------------------------------------------------------------------
    public Address getAddress(){return new Address(this.address);}
    //---------------------------------------------------------------------
    public void setAddress(Address a){
        this.address = (a == null) ? new Address() : a;
    }
    //---------------------------------------------------------------------
    public String toString(){return new String(this.name + " " + address.toString());}

}